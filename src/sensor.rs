use sensors::{FeatureType, Sensors, SubfeatureType};

pub struct Sensor {
    sub_feature: sensors::Subfeature,
}

impl Sensor {
    pub(crate) fn read(&self) -> i32 {
        self.sub_feature.get_value().expect("Failed to read sensor") as i32
    }
}

pub fn sensor(name: &str) -> Sensor {
    let sensors = Sensors::new();
    let chip = sensors
        .detected_chips(name)
        .expect("No sensor found")
        .next()
        .expect("No sensor found");

    let sub_feature = chip
        .into_iter()
        .filter(|f| (f.feature_type().eq(&FeatureType::SENSORS_FEATURE_TEMP)))
        .flatten()
        .filter(|sub| (sub.subfeature_type() == &SubfeatureType::SENSORS_SUBFEATURE_TEMP_INPUT))
        .next()
        .expect("No default subfeature found");

    Sensor { sub_feature }
}
