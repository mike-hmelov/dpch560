use hidapi::HidDevice;
use log::info;
use crate::hid;

const CH560_VENDOR_ID: u16 = 0x3633;
const CH560_PRODUCT_ID: u16 = 0x0005;
const MAGIC_HEADER: u8 = 16;
const CELSIUS: u8 = 19;

pub struct Display {
    hid: HidDevice,
}

pub fn display() -> Display {
    let hid = hid::init_hid(CH560_VENDOR_ID, CH560_PRODUCT_ID);

    hid.write(&[16, 170]).unwrap();
    Display { hid }
}

impl Display {
    pub(crate) fn write(&self, cpu_temp: i32, gpu_temp: i32) {
        info!("write to display cpu: {cpu_temp} gpu: {gpu_temp}");

        let cpu_usage = 31;
        let gpu_usage = 31;
        let temp_placeholder = 0;
        #[rustfmt::skip]
        let mut buf :[u8; 64] = [
            MAGIC_HEADER, CELSIUS, cpu_usage, temp_placeholder, temp_placeholder, temp_placeholder,
            CELSIUS, gpu_usage, temp_placeholder, temp_placeholder, temp_placeholder,
            0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        ];
        to_array(cpu_temp, 3, &mut buf);
        to_array(gpu_temp, 8, &mut buf);

        self.hid.write(&buf).unwrap();
    }
}

fn to_array(temp: i32, offset: i32, target: &mut [u8; 64]) {
    let mut current = temp;
    target[(offset + 2) as usize] = (current % 10) as u8;
    current /= 10;
    target[(offset + 1) as usize] = (current % 10) as u8;
    current /= 10;
    target[offset as usize] = (current % 10) as u8;
}
