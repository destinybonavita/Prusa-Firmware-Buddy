#include "store_instance.hpp"
#include <module/prusa/dock_position.hpp>
#include <module/prusa/tool_offset.hpp>

namespace config_store_ns {

static_assert(sizeof(CurrentStore) < (BANK_SIZE / 100) * 75, "EEPROM bank is almost full");
#if !defined(EEPROM_JOURNAL_SKIP_DUPLICITY_CHECK)
static_assert(journal::has_unique_items<CurrentStore, DeprecatedStore>(), "Just added items are causing collisions");
#endif

int32_t CurrentStore::get_extruder_fs_ref_value(uint8_t index) {
    switch (index) {
    case 0:
        return extruder_fs_ref_value_0.get();
    case 1:
        return extruder_fs_ref_value_1.get();
    case 2:
        return extruder_fs_ref_value_2.get();
    case 3:
        return extruder_fs_ref_value_3.get();
    case 4:
        return extruder_fs_ref_value_4.get();
    case 5:
        return extruder_fs_ref_value_5.get();
    default:
        assert(false && "invalid index");
        return 0;
    }
}

void CurrentStore::set_extruder_fs_ref_value(uint8_t index, int32_t value) {
    switch (index) {
    case 0:
        extruder_fs_ref_value_0.set(value);
        break;
    case 1:
        extruder_fs_ref_value_1.set(value);
        break;
    case 2:
        extruder_fs_ref_value_2.set(value);
        break;
    case 3:
        extruder_fs_ref_value_3.set(value);
        break;
    case 4:
        extruder_fs_ref_value_4.set(value);
        break;
    case 5:
        extruder_fs_ref_value_5.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

uint32_t CurrentStore::get_extruder_fs_value_span(uint8_t index) {
    switch (index) {
    case 0:
        return extruder_fs_value_span_0.get();
    case 1:
        return extruder_fs_value_span_1.get();
    case 2:
        return extruder_fs_value_span_2.get();
    case 3:
        return extruder_fs_value_span_3.get();
    case 4:
        return extruder_fs_value_span_4.get();
    case 5:
        return extruder_fs_value_span_5.get();
    default:
        assert(false && "invalid index");
        return 0;
    }
}

void CurrentStore::set_extruder_fs_value_span(uint8_t index, uint32_t value) {
    switch (index) {
    case 0:
        extruder_fs_value_span_0.set(value);
        break;
    case 1:
        extruder_fs_value_span_1.set(value);
        break;
    case 2:
        extruder_fs_value_span_2.set(value);
        break;
    case 3:
        extruder_fs_value_span_3.set(value);
        break;
    case 4:
        extruder_fs_value_span_4.set(value);
        break;
    case 5:
        extruder_fs_value_span_5.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

int32_t CurrentStore::get_side_fs_ref_value(uint8_t index) {
    switch (index) {
    case 0:
        return side_fs_ref_value_0.get();
    case 1:
        return side_fs_ref_value_1.get();
    case 2:
        return side_fs_ref_value_2.get();
    case 3:
        return side_fs_ref_value_3.get();
    case 4:
        return side_fs_ref_value_4.get();
    case 5:
        return side_fs_ref_value_5.get();
    default:
        assert(false && "invalid index");
        return 0;
    }
}

void CurrentStore::set_side_fs_ref_value(uint8_t index, int32_t value) {
    switch (index) {
    case 0:
        side_fs_ref_value_0.set(value);
        break;
    case 1:
        side_fs_ref_value_1.set(value);
        break;
    case 2:
        side_fs_ref_value_2.set(value);
        break;
    case 3:
        side_fs_ref_value_3.set(value);
        break;
    case 4:
        side_fs_ref_value_4.set(value);
        break;
    case 5:
        side_fs_ref_value_5.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

uint32_t CurrentStore::get_side_fs_value_span(uint8_t index) {
    switch (index) {
    case 0:
        return side_fs_value_span_0.get();
    case 1:
        return side_fs_value_span_1.get();
    case 2:
        return side_fs_value_span_2.get();
    case 3:
        return side_fs_value_span_3.get();
    case 4:
        return side_fs_value_span_4.get();
    case 5:
        return side_fs_value_span_5.get();
    default:
        assert(false && "invalid index");
        return 0;
    }
}

void CurrentStore::set_side_fs_value_span(uint8_t index, uint32_t value) {
    switch (index) {
    case 0:
        side_fs_value_span_0.set(value);
        break;
    case 1:
        side_fs_value_span_1.set(value);
        break;
    case 2:
        side_fs_value_span_2.set(value);
        break;
    case 3:
        side_fs_value_span_3.set(value);
        break;
    case 4:
        side_fs_value_span_4.set(value);
        break;
    case 5:
        side_fs_value_span_5.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

DockPosition CurrentStore::get_dock_position(uint8_t index) {
    switch (index) {
    case 0:
        return dock_position_0.get();
    case 1:
        return dock_position_1.get();
    case 2:
        return dock_position_2.get();
    case 3:
        return dock_position_3.get();
    case 4:
        return dock_position_4.get();
    case 5:
        return dock_position_5.get();
    default:
        assert(false && "invalid index");
        return {};
    }
}

void CurrentStore::set_dock_position(uint8_t index, DockPosition value) {
    switch (index) {
    case 0:
        dock_position_0.set(value);
        break;
    case 1:
        dock_position_1.set(value);
        break;
    case 2:
        dock_position_2.set(value);
        break;
    case 3:
        dock_position_3.set(value);
        break;
    case 4:
        dock_position_4.set(value);
        break;
    case 5:
        dock_position_5.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

ToolOffset CurrentStore::get_tool_offset(uint8_t index) {
    switch (index) {
    case 0:
        return tool_offset_0.get();
    case 1:
        return tool_offset_1.get();
    case 2:
        return tool_offset_2.get();
    case 3:
        return tool_offset_3.get();
    case 4:
        return tool_offset_4.get();
    case 5:
        return tool_offset_5.get();
    default:
        assert(false && "invalid index");
        return {};
    }
}

void CurrentStore::set_tool_offset(uint8_t index, ToolOffset value) {
    switch (index) {
    case 0:
        tool_offset_0.set(value);
        break;
    case 1:
        tool_offset_1.set(value);
        break;
    case 2:
        tool_offset_2.set(value);
        break;
    case 3:
        tool_offset_3.set(value);
        break;
    case 4:
        tool_offset_4.set(value);
        break;
    case 5:
        tool_offset_5.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

filament::Type CurrentStore::get_filament_type(uint8_t index) {
    switch (index) {
    case 0:
        return filament_type_0.get();
    case 1:
        return filament_type_1.get();
    case 2:
        return filament_type_2.get();
    case 3:
        return filament_type_3.get();
    case 4:
        return filament_type_4.get();
    case 5:
        return filament_type_5.get();
    default:
        assert(false && "invalid index");
        return {};
    }
}

void CurrentStore::set_filament_type(uint8_t index, filament::Type value) {
    switch (index) {
    case 0:
        filament_type_0.set(value);
        break;
    case 1:
        filament_type_1.set(value);
        break;
    case 2:
        filament_type_2.set(value);
        break;
    case 3:
        filament_type_3.set(value);
        break;
    case 4:
        filament_type_4.set(value);
        break;
    case 5:
        filament_type_5.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

float CurrentStore::get_nozzle_diameter(uint8_t index) {
    switch (index) {
    case 0:
        return nozzle_diameter_0.get();
    case 1:
        return nozzle_diameter_1.get();
    case 2:
        return nozzle_diameter_2.get();
    case 3:
        return nozzle_diameter_3.get();
    case 4:
        return nozzle_diameter_4.get();
    case 5:
        return nozzle_diameter_5.get();
    default:
        assert(false && "invalid index");
        return {};
    }
}

void CurrentStore::set_nozzle_diameter(uint8_t index, float value) {
    switch (index) {
    case 0:
        nozzle_diameter_0.set(value);
        break;
    case 1:
        nozzle_diameter_1.set(value);
        break;
    case 2:
        nozzle_diameter_2.set(value);
        break;
    case 3:
        nozzle_diameter_3.set(value);
        break;
    case 4:
        nozzle_diameter_4.set(value);
        break;
    case 5:
        nozzle_diameter_5.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

float CurrentStore::get_odometer_axis(uint8_t index) {

    switch (index) {
    case 0:
        return odometer_x.get();
    case 1:
        return odometer_y.get();
    case 2:
        return odometer_z.get();
    default:
        assert(false && "invalid index");
        return {};
    }
}

void CurrentStore::set_odometer_axis(uint8_t index, float value) {
    switch (index) {
    case 0:
        odometer_x.set(value);
        break;
    case 1:
        odometer_y.set(value);
        break;
    case 2:
        odometer_z.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

float CurrentStore::get_odometer_extruded_length(uint8_t index) {
    switch (index) {
    case 0:
        return odometer_extruded_length_0.get();
    case 1:
        return odometer_extruded_length_1.get();
    case 2:
        return odometer_extruded_length_2.get();
    case 3:
        return odometer_extruded_length_3.get();
    case 4:
        return odometer_extruded_length_4.get();
    case 5:
        return odometer_extruded_length_5.get();
    default:
        assert(false && "invalid index");
        return {};
    }
}

void CurrentStore::set_odometer_extruded_length(uint8_t index, float value) {
    switch (index) {
    case 0:
        odometer_extruded_length_0.set(value);
        break;
    case 1:
        odometer_extruded_length_1.set(value);
        break;
    case 2:
        odometer_extruded_length_2.set(value);
        break;
    case 3:
        odometer_extruded_length_3.set(value);
        break;
    case 4:
        odometer_extruded_length_4.set(value);
        break;
    case 5:
        odometer_extruded_length_5.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

uint32_t CurrentStore::get_odometer_toolpicks(uint8_t index) {
    switch (index) {
    case 0:
        return odometer_toolpicks_0.get();
    case 1:
        return odometer_toolpicks_1.get();
    case 2:
        return odometer_toolpicks_2.get();
    case 3:
        return odometer_toolpicks_3.get();
    case 4:
        return odometer_toolpicks_4.get();
    case 5:
        return odometer_toolpicks_5.get();
    default:
        assert(false && "invalid index");
        return {};
    }
}

void CurrentStore::set_odometer_toolpicks(uint8_t index, uint32_t value) {
    switch (index) {
    case 0:
        odometer_toolpicks_0.set(value);
        break;
    case 1:
        odometer_toolpicks_1.set(value);
        break;
    case 2:
        odometer_toolpicks_2.set(value);
        break;
    case 3:
        odometer_toolpicks_3.set(value);
        break;
    case 4:
        odometer_toolpicks_4.set(value);
        break;
    case 5:
        odometer_toolpicks_5.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

SelftestTool CurrentStore::get_selftest_result_tool(uint8_t index) {
    assert(index < config_store_ns::max_tool_count);
    return selftest_result.get().tools[index];
}

void CurrentStore::set_selftest_result_tool(uint8_t index, SelftestTool value) {
    assert(index < config_store_ns::max_tool_count);
    auto tmp = selftest_result.get();
    tmp.tools[index] = value;
    selftest_result.set(tmp);
}

Sheet CurrentStore::get_sheet(uint8_t index) {
    assert(index < config_store_ns::sheets_num);
    switch (index) {
    case 0:
        return sheet_0.get();
    case 1:
        return sheet_1.get();
    case 2:
        return sheet_2.get();
    case 3:
        return sheet_3.get();
    case 4:
        return sheet_4.get();
    case 5:
        return sheet_5.get();
    case 6:
        return sheet_6.get();
    case 7:
        return sheet_7.get();
    default:
        assert(false && "invalid index");
        return {};
    }
}

void CurrentStore::set_sheet(uint8_t index, Sheet value) {
    assert(index < config_store_ns::sheets_num);
    switch (index) {
    case 0:
        sheet_0.set(value);
        break;
    case 1:
        sheet_1.set(value);
        break;
    case 2:
        sheet_2.set(value);
        break;
    case 3:
        sheet_3.set(value);
        break;
    case 4:
        sheet_4.set(value);
        break;
    case 5:
        sheet_5.set(value);
        break;
    case 6:
        sheet_6.set(value);
        break;
    case 7:
        sheet_7.set(value);
        break;
    default:
        assert(false && "invalid index");
        return;
    }
}

input_shaper::Config CurrentStore::get_input_shaper_config() {
    input_shaper::Config config;
    if (input_shaper_axis_x_enabled.get()) {
        config.axis[X_AXIS] = input_shaper_axis_x_config.get();
    } else {
        config.axis[X_AXIS] = std::nullopt;
    }
    if (input_shaper_axis_y_enabled.get()) {
        config.axis[Y_AXIS] = input_shaper_axis_y_config.get();
    } else {
        config.axis[Y_AXIS] = std::nullopt;
    }
    if (input_shaper_weight_adjust_y_enabled.get()) {
        config.weight_adjust_y = input_shaper_weight_adjust_y_config.get();
    } else {
        config.weight_adjust_y = std::nullopt;
    }
    return config;
}

void CurrentStore::set_input_shaper_config(const input_shaper::Config &config) {
    if (config.axis[X_AXIS]) {
        input_shaper_axis_x_config.set(*config.axis[X_AXIS]);
        input_shaper_axis_x_enabled.set(true);
    } else {
        input_shaper_axis_x_enabled.set(false);
    }
    if (config.axis[Y_AXIS]) {
        input_shaper_axis_y_config.set(*config.axis[Y_AXIS]);
        input_shaper_axis_y_enabled.set(true);
    } else {
        input_shaper_axis_y_enabled.set(false);
    }
    if (config.weight_adjust_y) {
        input_shaper_weight_adjust_y_config.set(*config.weight_adjust_y);
        input_shaper_weight_adjust_y_enabled.set(true);
    } else {
        input_shaper_weight_adjust_y_enabled.set(false);
    }
}

} // namespace config_store_ns
