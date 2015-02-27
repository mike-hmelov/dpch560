use log::info;

pub struct Display {}

impl Display {
    pub(crate) fn write(&self, cpu_temp: i32, gpu_temp: i32) {
        info!("write to display cpu: {cpu_temp} gpu: {gpu_temp}")
    }
}

pub fn display() -> Display {
    Display {}
}
