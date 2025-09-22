use crate::sensor;
use crate::sensor::Sensor;

pub struct CPU {
    sensor: Sensor
}

impl CPU {
    pub(crate) fn usage(&self) -> u8 {
        0
    }
}

impl CPU {
    pub(crate) fn temperature(&self) -> i32 {
        self.sensor.read()
    }
}

impl CPU {
    pub fn new(sensor_name: &str) -> Self {
        let cpu_sensor = sensor::Sensor::new(sensor_name);

        CPU {
            sensor: cpu_sensor
        }
    }
}