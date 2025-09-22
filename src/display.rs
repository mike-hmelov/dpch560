use log::info;
use rusb::{Context, DeviceHandle, UsbContext};
use std::time::Duration;

const CH560_VENDOR_ID: u16 = 0x3633;
const CH560_PRODUCT_ID: u16 = 0x0005;

const INTERFACE: u8 = 0;
const ENDPOINT_OUT: u8 = 1;
const MAGIC_HEADER: u8 = 16;
const CELSIUS: u8 = 19;
const TIMEOUT: Duration = Duration::from_secs(1);

pub struct Display {
    device: DeviceHandle<Context>,
}

impl Display {
    pub fn new() -> Self {
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

        let header: [u8; 2] = [MAGIC_HEADER, 0xAA];

        device
            .write_interrupt(ENDPOINT_OUT, &header, TIMEOUT)
            .expect("unable to write to device");

        Display { device }
    }

    pub(crate) fn write(&self, cpu_temp: i32, cpu_usage: u8, gpu_temp: i32, gpu_usage: u8) {
        info!("write to display cpu: {cpu_temp} gpu: {gpu_temp}");

        let mut buf: [u8; 64] = [0; 64];
        buf[0] = MAGIC_HEADER;
        buf[1] = CELSIUS;
        buf[2] = cpu_usage / 10;
        write_bcd_3(cpu_temp, 3, &mut buf);
        buf[6] = CELSIUS;
        buf[7] = gpu_usage / 10;
        write_bcd_3(gpu_temp, 8, &mut buf);

        self.device
            .write_interrupt(ENDPOINT_OUT, &buf, TIMEOUT)
            .expect("unable to write to device");
    }
}

fn write_bcd_3(temp: i32, offset: usize, target: &mut [u8; 64]) {
    let mut t = temp.abs().min(999); // clamp to 0..999 to avoid wrap
    target[offset + 2] = (t % 10) as u8;
    t /= 10;
    target[offset + 1] = (t % 10) as u8;
    t /= 10;
    target[offset] = (t % 10) as u8;
}
