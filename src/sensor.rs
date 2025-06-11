use rand::random;

pub struct Sensor;


impl Sensor {
    pub(crate) fn read(&self) -> i32 {
        let a: i32 = random();
        if a > 0 { a % 100 } else { -a % 100 }
    }
}

pub fn sensor() -> Sensor {
    Sensor
}
