use crate::sensor;
use crate::sensor::Sensor;

pub struct GPU {
    sensor: Sensor,
}

impl GPU {
    pub(crate) fn usage(&self) -> u8 {
        0
    }
}

impl GPU {
    pub(crate) fn temperature(&self) -> i32 {
        self.sensor.read()
    }
}

impl GPU {
    pub fn new(sensor_name: &str) -> Self {
        let gpu_sensor = sensor::Sensor::new(sensor_name);
        GPU { sensor: gpu_sensor }
    }
}
