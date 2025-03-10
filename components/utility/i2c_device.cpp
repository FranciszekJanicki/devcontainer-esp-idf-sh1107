#include "i2c_device.hpp"

namespace Utility {

    I2CDevice::I2CDevice(i2c_master_dev_handle_t const i2c_device) noexcept : i2c_device_{i2c_device}
    {
        this->initialize();
    }

    void I2CDevice::transmit_dword(std::uint32_t const dword) const noexcept
    {
        this->transmit_dwords(std::array<std::uint32_t, 1UL>{dword});
    }

    void I2CDevice::transmit_word(std::uint16_t const word) const noexcept
    {
        this->transmit_words(std::array<std::uint16_t, 1UL>{word});
    }

    void I2CDevice::transmit_byte(std::uint8_t const byte) const noexcept
    {
        this->transmit_bytes(std::array<std::uint8_t, 1UL>{byte});
    }

    std::uint32_t I2CDevice::receive_dword() const noexcept
    {
        return this->receive_dwords<1UL>()[0];
    }

    std::uint16_t I2CDevice::receive_word() const noexcept
    {
        return this->receive_words<1UL>()[0];
    }

    std::uint8_t I2CDevice::receive_byte() const noexcept
    {
        return this->receive_bytes<1UL>()[0];
    }

    std::uint32_t I2CDevice::read_dword(std::uint8_t const reg_address) const noexcept
    {
        return this->read_dwords<1UL>(reg_address)[0];
    }

    std::uint16_t I2CDevice::read_word(std::uint8_t const reg_address) const noexcept
    {
        return this->read_words<1UL>(reg_address)[0];
    }

    std::uint8_t I2CDevice::read_byte(std::uint8_t const reg_address) const noexcept
    {
        return this->read_bytes<1UL>(reg_address)[0];
    }

    std::uint8_t I2CDevice::read_bits(std::uint8_t const reg_address,
                                      std::uint8_t const position,
                                      std::size_t const size) const noexcept
    {
        return Utility::read_bits(this->read_byte(reg_address), size, position);
    }

    bool I2CDevice::read_bit(std::uint8_t const reg_address, std::uint8_t const position) const noexcept
    {
        return Utility::read_bit(this->read_byte(reg_address), position);
    }

    void I2CDevice::write_dword(std::uint8_t const reg_address, std::uint32_t const dword) const noexcept
    {
        this->write_dwords(reg_address, std::array<std::uint32_t, 1UL>{dword});
    }

    void I2CDevice::write_word(std::uint8_t const reg_address, std::uint16_t const word) const noexcept
    {
        this->write_words(reg_address, std::array<std::uint16_t, 1UL>{word});
    }

    void I2CDevice::write_byte(std::uint8_t const reg_address, std::uint8_t const byte) const noexcept
    {
        this->write_bytes(reg_address, std::array<std::uint8_t, 1UL>{byte});
    }

    void I2CDevice::write_bits(std::uint8_t const reg_address,
                               std::uint8_t const bits,
                               std::uint8_t const position,
                               std::size_t const size) const noexcept
    {
        std::uint8_t write{this->read_byte(reg_address)};
        Utility::write_bits(write, bits, size, position);
        this->write_byte(reg_address, write);
    }

    void
    I2CDevice::write_bit(std::uint8_t const reg_address, bool const bit, std::uint8_t const position) const noexcept
    {
        std::uint8_t write{this->read_byte(reg_address)};
        Utility::write_bit(write, bit, position);
        this->write_byte(reg_address, write);
    }

    void I2CDevice::initialize() noexcept
    {
        if (this->i2c_device_ != nullptr) {
            // if (i2c_master_probe(this->i2c_device_->master_bus, this->i2c_device_->device_address, TIMEOUT) ==
            // ESP_OK) {
            this->initialized_ = true;
            //}
        }
    }

}; // namespace Utility
