extern crate daemonize;
extern crate log;

mod demonize;
mod display;
mod logging;
mod sensor;

use std::process::ExitCode;
use std::thread;
use std::time::Duration;

fn main() -> ExitCode {
    demonize::demonize();

    logging::setup_syslog();

    do_logic();

    ExitCode::SUCCESS
}

fn do_logic() {
    let display = display::display();
    let cpu_sensor = sensor::sensor("k10temp-pci-00c3");
    let gpu_sensor = sensor::sensor("amdgpu-pci-0300");

    loop {
        display.write(cpu_sensor.read(), 10, gpu_sensor.read(), 50);
        thread::sleep(Duration::from_secs(1));
    }
}

// TODO parse args
// TODO if demonize - run following code
// TODO define monitor usb ids
// TODO make full rebrand daemon
// TODO make rpm
// TODO make deb
// TODO make static or lib dependent
// TODO learn cpu usage
// TODO send real cpu usage to display
// TODO learn gpu usage
// TODO send readl gpu usage to display
// TODO make gpu configurable
// TODO make grade configurable
// TODO rotate logs
// TODO make ids parametrized (support other devices??)
// TODO find feature by name - rather then just [0]
