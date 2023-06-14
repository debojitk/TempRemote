

#pragma once

template <typename SensorModule, typename Value>
class Sensor {
public:
    Value       get() const           { return _s.get(); }
    bool        set(const Value& val) { return _s.set(val); }
    const char* error() const         { return _s.error(); }
    void        setup()               { _s.setup(); }
private:
    SensorModule _s;
};
