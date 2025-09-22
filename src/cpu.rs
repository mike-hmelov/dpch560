use crate::sensor;
use crate::sensor::Sensor;
use cpu_monitor::CpuInstant;

pub struct CPU {
    sensor: Sensor,
}

impl CPU {
    pub(crate) fn usage(&self, initial_instant: CpuInstant) -> u8 {
        let usage = (self.read_instant() - initial_instant).non_idle() * 100.0;

        (usage).round() as u8
    }

    pub fn read_instant(&self) -> CpuInstant {
        CpuInstant::now().expect("Failed to read cpu instant")
    }

    pub(crate) fn temperature(&self) -> i32 {
        self.sensor.read()
    }
}

impl CPU {
    pub fn new(sensor_name: &str) -> Self {
        let cpu_sensor = sensor::Sensor::new(sensor_name);

        CPU { sensor: cpu_sensor }
    }
}
