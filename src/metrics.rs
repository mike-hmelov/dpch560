pub trait MetricProvider {
    fn read(&self) -> i32;
}
