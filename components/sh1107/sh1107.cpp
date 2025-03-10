#include "sh1107.hpp"
#include "utility.hpp"

namespace SH1107 {

    SH1107::SH1107(SPIDevice&& spi_device,
                   Config const& config,
                   gpio_num_t const control_pin,
                   gpio_num_t const reset_pin) noexcept :
        control_pin_{control_pin}, reset_pin_{reset_pin}, spi_device_{std::forward<SPIDevice>(spi_device)}
    {
        this->initialize(config);
    }

    SH1107::~SH1107() noexcept
    {
        this->deinitialize();
    }

    void SH1107::display(std::uint8_t const* const byte_image, std::size_t const bytes) const noexcept
    {
        std::uint8_t const width = (SCREEN_WIDTH % 8 == 0) ? (SCREEN_WIDTH / 8) : (SCREEN_WIDTH / 8 + 1);
        std::uint8_t const height = SCREEN_HEIGHT;

        std::uint8_t column = 0U;
        std::uint8_t temp = 0U;

        this->transmit_data(0xb0);
        for (std::uint8_t j = 0U; j < height; j++) {
            // column = 63 - j;
            column = j;
            this->transmit_command(0x00 + (column & 0x0f));
            this->transmit_command(0x10 + (column >> 4));
            for (std::uint8_t i = 0U; i < width; i++) {
                temp = (byte_image[i + j * (width)]);
                temp = Utility::reflection(temp);
                this->transmit_data(temp);
            }
        }
    }

    void SH1107::transmit_data(std::uint8_t const byte) const noexcept
    {
        this->select_control_pad(ControlPad::DISPLAY_DATA);
        this->spi_device_.transmit_byte(byte);
    }

    void SH1107::transmit_command(std::uint8_t const byte) const noexcept
    {
        this->select_control_pad(ControlPad::COMMAND_DATA);
        this->spi_device_.transmit_byte(byte);
    }

    void SH1107::write_byte(std::uint8_t const reg_address, std::uint8_t const byte) const noexcept
    {
        this->select_control_pad(ControlPad::COMMAND_DATA);
        this->spi_device_.transmit_byte(reg_address);
        this->spi_device_.transmit_byte(byte);
    }

    void SH1107::initialize(Config const& config) noexcept
    {
        this->device_reset();
        this->display_off();

        this->send_lower_column_address_command(config.lower_column_address);
        this->send_higher_column_address_command(config.higher_column_address);
        this->send_page_address_command(config.page_address);
        this->set_display_start_line_register(config.display_start_line);
        this->set_contrast_control_register(config.contrast_control);
        this->send_normal_reverse_display_command(config.normal_reverse_display);
        this->set_multiplex_ratio_register(config.multiplex_ratio);
        this->set_display_offset_register(config.display_offset);
        this->set_clock_divide_osc_freq_register(config.clock_divide_osc_freq);
        this->set_charge_period_register(config.charge_period);
        this->set_vcom_deselect_level_register(config.vcom_deselect_level);
        this->set_dc_dc_control_mode_register(config.dc_dc_control_mode);

        this->display_on();
        this->initialized_ = true;
    }

    void SH1107::deinitialize() noexcept
    {
        this->device_reset();
        this->display_off();
        this->initialized_ = false;
    }

    void SH1107::device_reset() const noexcept
    {
        gpio_set_level(this->reset_pin_, 0U);
        vTaskDelay(pdMS_TO_TICKS(200U));
        gpio_set_level(this->reset_pin_, 1U);
        vTaskDelay(pdMS_TO_TICKS(200U));
    }

    void SH1107::display_on() const noexcept
    {
        this->send_display_on_off_command(DISPLAY_ON_OFF{.on_off = true});
    }

    void SH1107::display_off() const noexcept
    {
        this->send_display_on_off_command(DISPLAY_ON_OFF{.on_off = false});
    }

    void SH1107::select_control_pad(ControlPad const control_pad) const noexcept
    {
        gpio_set_level(this->control_pin_, std::to_underlying(control_pad));
    }

    void SH1107::send_lower_column_address_command(LOWER_COLUMN_ADDRESS const lower_column_address) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(lower_column_address));
    }

    void SH1107::send_higher_column_address_command(HIGHER_COLUMN_ADDRESS const higher_column_address) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(higher_column_address));
    }

    void SH1107::send_memory_addressing_mode_command(MEMORY_ADDRESSING_MODE const memory_addressing_mode) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(memory_addressing_mode));
    }

    void SH1107::send_segment_remap_command(SEGMENT_REMAP const segment_remap) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(segment_remap));
    }

    void SH1107::send_entire_display_on_off_command(ENTIRE_DISPLAY_ON_OFF const entire_display_on_off) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(entire_display_on_off));
    }

    void SH1107::send_normal_reverse_display_command(NORMAL_REVERSE_DISPLAY const normal_reverse_display) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(normal_reverse_display));
    }

    void SH1107::send_display_on_off_command(DISPLAY_ON_OFF const display_on_off) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(display_on_off));
    }

    void SH1107::send_page_address_command(PAGE_ADDRESS const page_address) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(page_address));
    }

    void SH1107::send_output_scan_direction_command(OUTPUT_SCAN_DIRECTION const output_scan_direction) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(output_scan_direction));
    }

    void SH1107::send_read_modify_write_command(READ_MODIFY_WRITE const read_modify_write) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(read_modify_write));
    }

    void SH1107::send_end_command(END const end) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(end));
    }

    void SH1107::send_nop_command(NOP const nop) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(nop));
    }

    void SH1107::send_write_display_data_command(WRITE_DISPLAY_DATA const write_display_data) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(write_display_data));
    }

    void SH1107::send_read_id_command(READ_ID const read_id) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(read_id));
    }

    void SH1107::send_read_display_data_command(READ_DISPLAY_DATA const read_display_data) const noexcept
    {
        this->transmit_command(std::bit_cast<std::uint8_t>(read_display_data));
    }

    void SH1107::set_contrast_control_register(CONTRAST_CONTROL const contrast_control) const noexcept
    {
        this->write_byte(std::to_underlying(RegAddress::CONTRAST_CONTROL),
                         std::bit_cast<std::uint8_t>(contrast_control));
    }

    void SH1107::set_multiplex_ratio_register(MULTIPLEX_RATIO const multiplex_ratio) const noexcept
    {
        this->write_byte(std::to_underlying(RegAddress::MULTIPLEX_RATIO), std::bit_cast<std::uint8_t>(multiplex_ratio));
    }

    void SH1107::set_display_offset_register(DISPLAY_OFFSET const display_offset) const noexcept
    {
        this->write_byte(std::to_underlying(RegAddress::DISPLAY_OFFSET), std::bit_cast<std::uint8_t>(display_offset));
    }

    void SH1107::set_dc_dc_control_mode_register(DC_DC_CONTROL_MODE const dc_dc_control_mode) const noexcept
    {
        this->write_byte(std::to_underlying(RegAddress::DC_DC_CONTROL_MODE),
                         std::bit_cast<std::uint8_t>(dc_dc_control_mode));
    }

    void SH1107::set_clock_divide_osc_freq_register(CLOCK_DIVIDE_OSC_FREQ const display_divide_osc_freq) const noexcept
    {
        this->write_byte(std::to_underlying(RegAddress::CLOCK_DIVIDE_OSC_FREQ),
                         std::bit_cast<std::uint8_t>(display_divide_osc_freq));
    }

    void SH1107::set_charge_period_register(CHARGE_PERIOD const charge_period) const noexcept
    {
        this->write_byte(std::to_underlying(RegAddress::CHARGE_PERIOD), std::bit_cast<std::uint8_t>(charge_period));
    }

    void SH1107::set_vcom_deselect_level_register(VCOM_DESELECT_LEVEL const vcom_deselect_level) const noexcept
    {
        this->write_byte(std::to_underlying(RegAddress::VCOM_DESELECT_LEVEL),
                         std::bit_cast<std::uint8_t>(vcom_deselect_level));
    }

    void SH1107::set_display_start_line_register(DISPLAY_START_LINE const display_start_line) const noexcept
    {
        this->write_byte(std::to_underlying(RegAddress::DISPLAY_START_LINE),
                         std::bit_cast<std::uint8_t>(display_start_line));
    }

}; // namespace SH1107