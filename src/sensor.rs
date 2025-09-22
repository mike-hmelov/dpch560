use sensors::{FeatureType, Sensors, SubfeatureType};
use crate::metrics::MetricProvider;

pub struct Sensor {
    sub_feature: sensors::Subfeature,
}

impl MetricProvider for Sensor {
    fn read(&self) -> i32 {
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
        .find(|f| f.feature_type() == &FeatureType::SENSORS_FEATURE_TEMP)
        .into_iter()
        .flat_map(|f| f) // iterate subfeatures of the found temp feature
        .find(|sub| sub.subfeature_type() == &SubfeatureType::SENSORS_SUBFEATURE_TEMP_INPUT)
        .expect("No default subfeature found");

    Sensor { sub_feature }
}
