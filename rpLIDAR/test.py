import rclpy

from rclpy.node import Node
from sensor_msgs.msg import LaserScan

class MinimalSubscriber(Node):

    def __init__(self):
        super().__init__('minimal_subscriber')
        self.subscription = self.create_subscription(
            LaserScan,
            '/scan',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg:LaserScan):
        valid_ranges = [r for r in msg.ranges if msg.range_min < r < msg.range_max]
        print(f'number of valid points: {len(valid_ranges)}')
        rounded_values = [f'{round(r, 3)}m' for r in valid_ranges[:10]]
        print(rounded_values)

def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = MinimalSubscriber()

    rclpy.spin(minimal_subscriber)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()