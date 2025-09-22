extern crate daemonize;
extern crate log;

mod demonize;
mod display;
mod logging;
mod sensor;
mod metrics;

use std::process::ExitCode;
use std::thread;
use std::time::Duration;
use metrics::MetricProvider;

struct Config {
    run_as_daemon: bool,
    cpu_sensor: String,
    gpu_sensor: String,
    update_interval: u64,
}

fn main() -> ExitCode {
    let config = Config {
        run_as_daemon: true,
        cpu_sensor: "k10temp-pci-00c3".to_string(),
        gpu_sensor: "amdgpu-pci-0300".to_string(),
        update_interval: 1,
    };

    if config.run_as_daemon {
        demonize::demonize();
    }

    logging::setup_syslog();

    do_logic(config);

    ExitCode::SUCCESS
}

fn do_logic(config: Config) {
    let display = display::Display::new();
    let cpu_sensor = sensor::sensor(config.cpu_sensor.as_str());
    let gpu_sensor = sensor::sensor(config.gpu_sensor.as_str());

    // 1 == 1
    // 10 == 2
    // 11 == 3
    // 100 == 4
    // 101 == 5
    // 110 == 6
    // 111 == 7
    // 1000 == 8
    // 1001 == 9
    // 1010 == 10
    let usage = 70;
    loop {
        display.write(cpu_sensor.read(), usage, gpu_sensor.read(), usage);
        thread::sleep(Duration::from_secs(config.update_interval));
        // usage += 1;
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
