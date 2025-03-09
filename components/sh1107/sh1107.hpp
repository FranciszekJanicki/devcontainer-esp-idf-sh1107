#ifndef SH1107_HPP
#define SH1107_HPP

#include "driver/gpio.h"
#include "sh1107_config.hpp"
#include "sh1107_registers.hpp"
#include "spi_device.hpp"

namespace SH1107 {

    struct SH1107 {
    public:
        using SPIDevice = Utility::SPIDevice;

        SH1107() noexcept = default;
        SH1107(SPIDevice&& spi_device,
               Config const& config,
               gpio_num_t const control_pin,
               gpio_num_t const reset_pin) noexcept;

        SH1107(SH1107 const& other) = delete;
        SH1107(SH1107&& other) noexcept = default;

        SH1107& operator=(SH1107 const& other) = delete;
        SH1107& operator=(SH1107&& other) noexcept = default;

        ~SH1107() noexcept;

    private:
        void transmit_byte(ControlPad const control_pad, std::uint8_t const byte) const noexcept;

        template <std::size_t SIZE>
        void transmit_bytes(ControlPad const control_pad, std::array<std::uint8_t, SIZE> const& bytes) const noexcept;

        void write_byte(std::uint8_t const reg_address, std::uint8_t const byte) const noexcept;

        template <std::size_t SIZE>
        void write_bytes(std::uint8_t const reg_address, std::array<std::uint8_t, SIZE> const& bytes) const noexcept;

        void initialize(Config const& config) noexcept;
        void deinitialize() noexcept;

        void device_reset() const noexcept;

        void display_on() const noexcept;
        void display_off() const noexcept;

        void select_control_pad(ControlPad const control_pad) const noexcept;

        void send_lower_column_address_command(LOWER_COLUMN_ADDRESS const lower_column_address) const noexcept;
        void send_higher_column_address_command(HIGHER_COLUMN_ADDRESS const higher_column_address) const noexcept;
        void send_memory_addressing_mode_command(MEMORY_ADDRESSING_MODE const memory_addressing_mode) const noexcept;
        void send_segment_remap_command(SEGMENT_REMAP const segment_remap) const noexcept;
        void send_entire_display_on_off_command(ENTIRE_DISPLAY_ON_OFF const entire_display_on_off) const noexcept;
        void send_normal_reverse_display_command(NORMAL_REVERSE_DISPLAY const normal_reverse_display) const noexcept;
        void send_display_on_off_command(DISPLAY_ON_OFF const display_on_off) const noexcept;
        void send_page_address_command(PAGE_ADDRESS const page_address) const noexcept;
        void send_output_scan_direction_command(OUTPUT_SCAN_DIRECTION const output_scan_direction) const noexcept;
        void
        send_read_modify_write_command(READ_MODIFY_WRITE const read_modify_write = READ_MODIFY_WRITE{}) const noexcept;
        void send_end_command(END const end = END{}) const noexcept;
        void send_nop_command(NOP const nop = NOP{}) const noexcept;
        void send_write_display_data_command(WRITE_DISPLAY_DATA const write_display_data) const noexcept;
        void send_read_id_command(READ_ID const read_id) const noexcept;
        void send_read_display_data_command(READ_DISPLAY_DATA const read_display_data) const noexcept;

        void set_contrast_control_register(CONTRAST_CONTROL const contrast_control) const noexcept;
        void set_multiplex_ratio_register(MULTIPLEX_RATIO const multiplex_ratio) const noexcept;
        void set_display_offset_register(DISPLAY_OFFSET const display_offset) const noexcept;
        void set_dc_dc_control_mode_register(DC_DC_CONTROL_MODE const dc_dc_control_mode) const noexcept;
        void set_clock_divide_osc_freq_register(CLOCK_DIVIDE_OSC_FREQ const display_divide_osc_freq) const noexcept;
        void set_charge_period_register(CHARGE_PERIOD const charge_period) const noexcept;
        void set_vcom_deselect_level_register(VCOM_DESELECT_LEVEL const vcom_deselect_level) const noexcept;
        void set_display_start_line_register(DISPLAY_START_LINE const display_start_line) const noexcept;

        bool initialized_{false};

        gpio_num_t control_pin_{};
        gpio_num_t reset_pin_{};

        SPIDevice spi_device_{};
    };

    template <std::size_t SIZE>
    inline void SH1107::transmit_bytes(ControlPad const control_pad,
                                       std::array<std::uint8_t, SIZE> const& bytes) const noexcept
    {
        this->select_control_pad(control_pad);
        this->spi_device_.transmit_bytes(bytes);
    }

    template <std::size_t SIZE>
    inline void SH1107::write_bytes(std::uint8_t const reg_address,
                                    std::array<std::uint8_t, SIZE> const& bytes) const noexcept
    {
        this->select_control_pad(ControlPad::COMMAND_DATA);
        this->spi_device_.transmit_byte(reg_address);
        this->spi_device_.transmit_bytes(bytes);
    }

}; // namespace SH1107

#endif // SH1107_HPP