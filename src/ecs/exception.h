#include <stdexcept>

namespace mkr {
class missing_component : public std::runtime_error {
public:
    missing_component() : std::runtime_error("missing component") {}
    virtual ~missing_component() {}
};
}