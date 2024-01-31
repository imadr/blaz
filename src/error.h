#pragma once

#include "types.h"

namespace blaz {

struct Error {
    bool m_is_error = false;
    str m_message;

    Error() : m_message(""), m_is_error(false) {
    }

    Error(str message) : m_message(message), m_is_error(true) {
    }

    str message() {
        return m_message;
    }

    operator bool() const {
        return m_is_error;
    }
};

}  // namespace blaz