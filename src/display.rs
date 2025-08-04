use log::info;
use rusb::{Context, DeviceHandle, UsbContext};
use std::time::Duration;

const CH560_VENDOR_ID: u16 = 0x3633;
const CH560_PRODUCT_ID: u16 = 0x0005;

const INTERFACE: u8 = 0;
const ENDPOINT_OUT: u8 = 1;
const MAGIC_HEADER: u8 = 16;
const CELSIUS: u8 = 19;
const ZERO_BUF: [u8; 1] = [0];
const TIMEOUT: Duration = Duration::from_secs(1);

pub struct Display {
    device: DeviceHandle<Context>,
}

pub fn display() -> Display {
    let context = Context::new().expect("cannot open libusb context");

    let device = context
        .open_device_with_vid_pid(CH560_VENDOR_ID, CH560_PRODUCT_ID)
        .expect("cannot get device");

    if device
        .kernel_driver_active(INTERFACE)
        .expect("cannot get kernel driver")
    {
        device
            .detach_kernel_driver(INTERFACE)
            .expect("cannot detach kernel driver");
    }
    device
        .claim_interface(INTERFACE)
        .expect("unable to claim interface");

    let header: [u8; 2] = [16, 170];

    device
        .write_interrupt(ENDPOINT_OUT, &header, TIMEOUT)
        .expect("unable to write to device");

    Display { device }
}

impl Display {
    pub(crate) fn write(&self, cpu_temp: i32, gpu_temp: i32) {
        info!("write to display cpu: {cpu_temp} gpu: {gpu_temp}");

        let cpu_usage = 31;
        let gpu_usage = 42;
        let temp_placeholder = 0;
        #[rustfmt::skip]
        let mut buf :[u8; 11] = [
            MAGIC_HEADER, CELSIUS, cpu_usage, temp_placeholder, temp_placeholder, temp_placeholder,
            CELSIUS, gpu_usage, temp_placeholder, temp_placeholder, temp_placeholder
        ];
        to_array(cpu_temp, 3, &mut buf);
        to_array(gpu_temp, 8, &mut buf);

        let body: Vec<u8> = buf
            .into_iter()
            .chain(ZERO_BUF.into_iter().cycle().take(53))
            .collect();

        self.device
            .write_interrupt(ENDPOINT_OUT, &body, TIMEOUT)
            .expect("unable to write to device");
    }
}

fn to_array(temp: i32, offset: i32, target: &mut [u8; 11]) {
    let mut current = temp;
    target[(offset + 2) as usize] = (current % 10) as u8;
    current /= 10;
    target[(offset + 1) as usize] = (current % 10) as u8;
    current /= 10;
    target[offset as usize] = (current % 10) as u8;
}
