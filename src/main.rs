extern crate daemonize;
extern crate log;

mod cpu;
mod demonize;
mod display;
mod gpu;
mod logging;
mod sensor;

use std::process::ExitCode;
use std::thread;
use std::time::Duration;

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
    let cpu = cpu::CPU::new(config.cpu_sensor.as_str());
    let gpu = gpu::GPU::new(config.gpu_sensor.as_str());

    let mut cpu_instant = cpu.read_instant();
    loop {
        display.write(
            cpu.temperature(),
            cpu.usage(cpu_instant),
            gpu.temperature(),
            gpu.usage(),
        );
        cpu_instant = cpu.read_instant();
        thread::sleep(Duration::from_secs(config.update_interval));
    }
}

// TODO parse args
// TODO if demonize - run following code
// TODO define monitor usb ids
// TODO make full rebrand daemon
// TODO make rpm
// TODO make deb
// TODO make static or lib dependent
// TODO make gpu configurable
// TODO make gpu sensor configurable and resolvable
// TODO resolve cpu
// TODO make cpu sensor configurable and resolvable
// TODO make grade configurable
// TODO rotate logs
// TODO make ids parametrized (support other devices??)
// TODO find feature by name - rather then just [0]
// TODO resolve real pci device for gpu usage metric
