use hidapi::{HidApi, HidDevice};

pub fn init_hid(vendor: u16, product: u16) -> HidDevice {
    let api = HidApi::new().unwrap();
    let device = api.open(vendor, product).unwrap();

    device
}
