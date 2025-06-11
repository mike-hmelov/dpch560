extern crate daemonize;
extern crate log;

mod demonize;
mod display;
mod logging;
mod sensor;
mod hid;

use std::process::ExitCode;
use std::thread::sleep;
use std::time::Duration;

fn main() -> ExitCode {
    demonize::demonize();

    logging::setup_syslog();

    do_logic();

    ExitCode::SUCCESS
}

fn do_logic() {
    let display = display::display();
    let cpu_sensor = sensor::sensor();
    let gpu_sensor = sensor::sensor();

    loop {
        display.write(cpu_sensor.read(), gpu_sensor.read());
        sleep(Duration::from_secs(1));
    }

    // Display display(appRuntime.log_stream);
    // Sensor cpu_sensor(appRuntime.log_stream, appConfig.fCpuFunction, appConfig.fCpuSensorName, appConfig.fCpuSensorFunction);
    // Sensor gpu_sensor(appRuntime.log_stream, appConfig.fGpuFunction, appConfig.fGpuSensorName, appConfig.fGpuSensorFunction);
    //
    // if(!display.Valid()){
    //     return -4;
    // }
    // if(!cpu_sensor.Init() || !gpu_sensor.Init())
    // {
    //     return -5;
    // }
    // /* This global variable can be changed in function handling signal */
    // running = 1;
    //
    // unsigned char cpu_usage = 0;
    // unsigned char gpu_usage = 0;
    //
    // /* Never ending loop of server */
    // while (running == 1) {
    //     display.Write(cpu_usage, cpu_sensor.Read(), gpu_usage, gpu_sensor.Read());
    //
    //     /* Real server should use select() or poll() for waiting at
    //      * asynchronous event. Note: sleep() is interrupted, when
    //      * signal is received. */
    //     sleep(appConfig.Delay());
    // }
    //
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
