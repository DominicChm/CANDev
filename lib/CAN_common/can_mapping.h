#include <Arduino.h>

/**
 * Contains a pointer to some data, as well as the size of said data.
 * User-defined either manually and/or using the provided `link()` helper.
 * Fed into the CANMappings.
 *
 * Example usage:
 * ```
 * double randomval = 0;
 * can_mapping_t { &random_value, sizeof(random_value) }
 * ```
 */
struct can_mapping_t {
    void* data;
    uint8_t size;
};

/**
 * Helper to create a can_mapping_t from
 * a variable reference, without needing to repeat the field
 * or "sizeof". Uses CPP template deduction with sizeof.
 */
template <typename T>
constexpr can_mapping_t link(T& data) {
    static_assert(sizeof(T) <= 8, "CAN data types must be less than 8 bytes large");
    return can_mapping_t{&data, sizeof(T)};
}

/**
 * Wraps mappings (CAN address -> data pointer) in a generic way.
 * Allows module mappings to be used by multiple interfaces without
 * needing to redefine pointers to data.
 *
 * IMPORTANT: THE OFFSET OF A VALUE'S CAN ADDRESS IS DEFINED BY ITS INDEX IN THE MAPPINGS ARRAY
 *
 * base_struct - Ptr to the struct mapped pointers are within. Used to create offsets
 * mappings - array of can_mapping_t
 */
class CANMappings {
   public:
    const can_mapping_t* mappings;
    const size_t num_mappings;

    template <size_t NMappings>
    CANMappings(const can_mapping_t (&mappings)[NMappings]) : mappings(mappings), num_mappings(NMappings) {
    }

    /**
     *  Converts a CAN address offset to a pointer to the data it maps to.
     */
    void* address_offset_to_ptr(void* data_root, uint32_t offset) {
        if (offset >= num_mappings) return 0;

        // Use the provided mappings and their root address to
        // create a corresponding pointer into the passed memory.
        return mapping_to_ptr(data_root, &mappings[offset]);
    }

    size_t address_offset_to_size(uint32_t offset) {
        offset--;  // Data offsets start at Address + 1 (Address + 0 is reserved for framework)
        if (offset >= num_mappings) return 0;

        return mappings[offset].size;
    }

    void* mapping_to_ptr(void* data_root, const can_mapping_t* mapping) {
        if (mapping == nullptr) return 0;

        // Use the provided mappings and their root address to
        // create a corresponding pointer into the passed memory.
        return (void*)((size_t)data_root + (size_t)mapping->data);
    }

    template <typename T>
    static T* map_root() {
        return (T*)0;
    }
};
