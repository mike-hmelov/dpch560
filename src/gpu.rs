use crate::sensor;
use crate::sensor::Sensor;
use std::fs::read_to_string;

pub struct GPU {
    sensor: Sensor,
    usage_file: String,
}

fn find_card(path: &str) -> Option<String> {
    if let Ok(data) = read_to_string(format!("{path}/uevent")) {
        let driver = data.lines().next()?;
        if driver.ends_with("amdgpu") {
            return Some(format!("{path}/gpu_busy_percent"));
        }
    }

    None
}

impl GPU {
    pub(crate) fn usage(&self) -> u8 {
        let data = read_to_string(&self.usage_file).expect("Failed to get GPU usage (AMD)");

        data.trim_end().parse::<u8>().unwrap()
    }

    pub(crate) fn temperature(&self) -> i32 {
        self.sensor.read()
    }

    pub fn new(sensor_name: &str) -> Self {
        let gpu_sensor = sensor::Sensor::new(sensor_name);
        let pci_address = "0000:03:00.0";
        let path = format!("/sys/bus/pci/devices/{pci_address}");

        let usage_file = find_card(&path).expect("Failed access GPU (AMD) PCI_ADDR={pci_address}");

        GPU {
            sensor: gpu_sensor,
            usage_file,
        }
    }
}
