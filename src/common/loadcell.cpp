#include "loadcell.hpp"
#include "bsod_gui.hpp"
#include "error_codes.hpp"
#include "gpio.h"
#include "metric.h"
#include "bsod.h"
#include <cmath> //isnan
#include <algorithm>
#include <numeric>
#include <limits>
#include "timing.h"
#include "log.h"
#include "probe_position_lookback.hpp"
#include "bsod_gui.hpp"
#include "config_features.h"
#if ENABLED(POWER_PANIC)
    #include "power_panic.hpp"
#endif // POWER_PANIC
#include "../Marlin/src/module/planner.h"
#include "../Marlin/src/module/endstops.h"

LOG_COMPONENT_DEF(Loadcell, LOG_SEVERITY_INFO);

Loadcell loadcell;
static metric_t metric_loadcell = METRIC("loadcell", METRIC_VALUE_CUSTOM, 0, METRIC_HANDLER_DISABLE_ALL);
static metric_t metric_loadcell_hp = METRIC("loadcell_hp", METRIC_VALUE_FLOAT, 0, METRIC_HANDLER_DISABLE_ALL);
static metric_t metric_loadcell_xy = METRIC("loadcell_xy", METRIC_VALUE_FLOAT, 0, METRIC_HANDLER_DISABLE_ALL);
static metric_t metric_loadcell_age = METRIC("loadcell_age", METRIC_VALUE_INTEGER, 0, METRIC_HANDLER_DISABLE_ALL);

// To be used by sensor info screen so we don't have to parse the CUSTOM_VALUE from the loadcell metric
static metric_t metric_loadcell_value = METRIC("loadcell_value", METRIC_VALUE_FLOAT, 0, METRIC_HANDLER_DISABLE_ALL);

Loadcell::Loadcell()
    : scale(1)
    , thresholdStatic(NAN)
    , thresholdContinuous(NAN)
    , hysteresis(0)
    , failsOnLoadAbove(INFINITY)
    , failsOnLoadBelow(-INFINITY)
    , loadcellRaw(0)
    , endstop(false)
    , isSignalEventConfigured(false)
    , highPrecision(false)
    , tareMode(TareMode::Static)
    , offset(0)
    , z_filter()
    , xy_filter() {}

void Loadcell::ConfigureSignalEvent(osThreadId threadId, int32_t signal) {
    this->threadId = threadId;
    this->signal = signal;
    isSignalEventConfigured = 1;
}

void Loadcell::Tare(TareMode mode, bool wait) {
    if (!isSignalEventConfigured)
        bsod("loadcell signal not configured");

    // ensure high-precision mode is enabled when taring
    if (!highPrecision)
        bsod("high precision not enabled during tare");

    if (tareCount != 0)
        bsod("loadcell tare already requested");

    // discard the current sample: it could have been started long ago
    if (wait)
        WaitForNextSample();

    if (endstops.is_z_probe_enabled() && (endstop || xy_endstop))
        fatal_error("LOADCELL", "Tare under load");

    tareMode = mode;

    // request tare from ISR routine
    int requestedTareCount = tareMode == TareMode::Continuous ? std::max(z_filter.SETTLING_TIME, xy_filter.SETTLING_TIME) : 4;
    tareSum = 0;
    tareCount = requestedTareCount;

    // wait untill wa have all the samples that were requested
    while (tareCount != 0) {
        osEvent evt = osSignalWait(signal, 500);
        if (evt.status != osEventSignal) {
            // Power panic during MBL causes returning osErrorValue or osEventTimeout
            // Raising redscreen during AC power fault breaks the power panic resume cycle after restart
            // Loadcell values are irelevant here, because MBL will be restarted after power up
#if ENABLED(POWER_PANIC)
            if (power_panic::ac_fault_triggered && power_panic::is_ac_fault_active()) {
                log_info(Loadcell, "PowerPanic triggered during loadcell tare operation - tare cycle was broken");
                tareCount = 0;
                break;
            }
#endif                      // POWER_PANIC
#if !PRINTER_IS_PRUSA_MK3_5 // TODO fix error codes
            fatal_error(ErrCode::ERR_SYSTEM_LOADCELL_TARE_FAILED);
#endif
        }
    }

    offset = tareSum / requestedTareCount;

    endstop = false;
    xy_endstop = false;
}

void Loadcell::Clear() {
    tareCount = 0;
    loadcellRaw = 0;
    offset = 0;
    reset_filters();
    endstop = false;
    xy_endstop = false;
}

void Loadcell::reset_filters() {
    z_filter.reset();
    xy_filter.reset();
}

bool Loadcell::GetMinZEndstop() const {
    return endstop && endstops.is_z_probe_enabled();
}

bool Loadcell::GetXYEndstop() const {
    return xy_endstop;
}

void Loadcell::SetScale(float scale) {
    this->scale = scale;
}

void Loadcell::SetHysteresis(float hysteresis) {
    this->hysteresis = hysteresis;
}

float Loadcell::GetScale() const {
    return scale;
}

float Loadcell::GetHysteresis() const {
    return hysteresis;
}

int32_t Loadcell::get_raw_value() const {
    return loadcellRaw;
}

bool Loadcell::IsSignalConfigured() const {
    return isSignalEventConfigured;
}

void Loadcell::SetFailsOnLoadAbove(float failsOnLoadAbove) {
    this->failsOnLoadAbove = failsOnLoadAbove;
}

float Loadcell::GetFailsOnLoadAbove() const {
    return failsOnLoadAbove;
}

void Loadcell::set_xy_endstop(const bool enabled) {
    xy_endstop_enabled = enabled;
}

void Loadcell::ProcessSample(int32_t loadcellRaw, uint32_t time_us) {
    this->loadcellRaw = loadcellRaw;
    z_filter.filter(loadcellRaw);
    xy_filter.filter(loadcellRaw);

    float load = get_tared_z_load();

    int32_t ticks_us_from_now = ticks_diff(time_us, ticks_us());
    int32_t ticks_ms_from_now = ticks_us_from_now / 1000;
    uint32_t timestamp_ms = ticks_ms() + ticks_ms_from_now;

    last_sample_time = timestamp_ms;

    metric_record_custom_at_time(&metric_loadcell, timestamp_ms, " r=%ii,o=%ii,s=%0.4f", loadcellRaw, offset, (double)scale);
    metric_record_float(&metric_loadcell_value, load);

    const float loadHighPass = get_filtered_z_load();
    metric_record_float_at_time(&metric_loadcell_hp, timestamp_ms, loadHighPass);

    const float filtered_load_xy = get_filtered_xy();
    metric_record_float_at_time(&metric_loadcell_xy, timestamp_ms, filtered_load_xy);

    metric_record_integer_at_time(&metric_loadcell_age, timestamp_ms, ticks_us_from_now);

    float z_pos = buddy::probePositionLookback.get_position_at(time_us, []() { return planner.get_axis_position_mm(AxisEnum::Z_AXIS); });
    if (!std::isnan(z_pos)) {
        analysis.StoreSample(z_pos, load);
    } else {
        log_warning(Loadcell, "Got NaN z-coordinate; skipping (age=%dus)", ticks_us_from_now);
    }

    if (!std::isfinite(load)) {
#if !PRINTER_IS_PRUSA_MK3_5 // TODO fix error codes
        fatal_error(ErrCode::ERR_SYSTEM_LOADCELL_INFINITE_LOAD);
#endif
    }

    // Trigger Z endstop/probe
    float loadForEndstops = tareMode == TareMode::Static ? load : loadHighPass;
    float threshold = GetThreshold(tareMode);

    if (Endstops::is_z_probe_enabled()) {
#if 0
      // TODO: temporarily disabled for release until true overloads are resolved
      // load is negative, so flip the signs accordingly just below
        if ((isfinite(failsOnLoadAbove) && loadForEndstops < -failsOnLoadAbove)
            || (isfinite(failsOnLoadBelow) && loadForEndstops > -failsOnLoadBelow))
            fatal_error("LOADCELL", "Loadcell overload");
#endif
    }

    if (endstop) {
        if (loadForEndstops >= (threshold + hysteresis)) {
            endstop = false;
        }
        buddy::hw::zMin.isr();
    } else {
        if (loadForEndstops <= threshold) {
            endstop = true;
            buddy::hw::zMin.isr();
        }
    }

    // Trigger XY endstop/probe
    if (xy_endstop_enabled) {
        // Everything as absolute values, watch for changes.
        // Load perpendicular to the sensor sense vector is not guaranteed to have defined sign.
        if (abs(filtered_load_xy) > abs(XY_PROBE_THRESHOLD)) {
            xy_endstop = true;
            buddy::hw::zMin.isr();
        }
        if (abs(filtered_load_xy) < abs(XY_PROBE_THRESHOLD) - abs(XY_PROBE_HYSTERESIS)) {
            xy_endstop = false;
            buddy::hw::zMin.isr();
        }
    }

    if (tareCount != 0) {
        tareSum += loadcellRaw;
        tareCount -= 1;
    }

    if (isSignalEventConfigured) {
        osSignalSet(threadId, signal);
    }
}

int32_t Loadcell::WaitForNextSample() {
    if (!isSignalEventConfigured)
        bsod("loadcell signal not configured");

    // hx717: output settling time is 400 ms (for reset, channel change, gain change)
    // therefore 600 ms should be safe and if it takes longer, it is most likely an error
    auto result = osSignalWait(signal, 600);
    if (result.status != osEventSignal && !planner.draining()) {
#if !PRINTER_IS_PRUSA_MK3_5 // TODO fix error codes
        fatal_error(ErrCode::ERR_SYSTEM_LOADCELL_TIMEOUT);
#endif
    }
    return loadcellRaw;
}

void Loadcell::HomingSafetyCheck() const {
    static constexpr uint32_t MAX_LOADCELL_DATA_AGE_WHEN_HOMING = 100;
    if (ticks_ms() - last_sample_time > MAX_LOADCELL_DATA_AGE_WHEN_HOMING) {
        fatal_error(ErrCode::ERR_ELECTRO_HOMING_ERROR_Z);
    }
}

/**
 * @brief Create object enforcing error when positive load value is too big
 *
 * Sets grams threshold when created, restores to original when destroyed.
 * @param grams
 * @param enable Enable condition. Useful if you want to create enforcer based on condition.
 *              You can not put object simply inside if block, because you unintentionally also
 *              limit its scope.
 *   @arg @c true Normal operation
 *   @arg @c false Do not set grams threshold when created. (Doesn't affect destruction.)
 */

Loadcell::FailureOnLoadAboveEnforcer Loadcell::CreateLoadAboveErrEnforcer(bool enable, float grams) {
    return Loadcell::FailureOnLoadAboveEnforcer(*this, enable, grams);
}

/*****************************************************************************/
// IFailureEnforcer
Loadcell::IFailureEnforcer::IFailureEnforcer(Loadcell &lcell, float oldErrThreshold)
    : lcell(lcell)
    , oldErrThreshold(oldErrThreshold) {
}

/**
 *
 * @param lcell
 * @param grams
 * @param enable
 */
Loadcell::FailureOnLoadAboveEnforcer::FailureOnLoadAboveEnforcer(Loadcell &lcell, bool enable, float grams)
    : IFailureEnforcer(lcell, lcell.GetFailsOnLoadAbove()) {
    if (enable) {
        lcell.SetFailsOnLoadAbove(grams);
    }
}
Loadcell::FailureOnLoadAboveEnforcer::~FailureOnLoadAboveEnforcer() {
    lcell.SetFailsOnLoadAbove(oldErrThreshold);
}

/**
 * @brief Create object enabling high precision mode
 *
 * Enables high precision mode and tare continuously when created so lower threshold can be used,
 * disables high precision mode when destroyed.
 * @param enable Enable condition. Useful if you want to create enforcer based on condition.
 *              You can not put object simply inside if block, because you unintentionally also
 *              limit its scope.
 *   @arg @c true Normal operation
 *   @arg @c false Do not enable high precision mode and tare when created. (Doesn't affect destruction.)
 */
Loadcell::HighPrecisionEnabler::HighPrecisionEnabler(Loadcell &lcell,
    bool enable)
    : m_lcell(lcell) {
    if (enable) {
        m_lcell.EnableHighPrecision();
    }
}

Loadcell::HighPrecisionEnabler::~HighPrecisionEnabler() {
    m_lcell.DisableHighPrecision();
}
