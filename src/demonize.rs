use daemonize::{Daemonize, Stdio};

pub fn demonize() {
    if false {
        let daemonize = Daemonize::new()
            .working_directory("/")
            .stdout(Stdio::devnull())
            .stderr(Stdio::devnull())
            .chown_pid_file(true)
            .user("nobody")
            .group("daemon")
            .umask(0);

        match daemonize.start() {
            Ok(_) => {}
            Err(_) => panic!("Failed to daemonize"),
        }
    }
}
