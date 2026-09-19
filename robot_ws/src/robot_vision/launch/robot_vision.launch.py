from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch_xml.launch_description_sources import XMLLaunchDescriptionSource


def generate_launch_description():
    rosbridge = IncludeLaunchDescription(
        XMLLaunchDescriptionSource(
            PathJoinSubstitution([
                FindPackageShare('rosbridge_server'),
                'launch',
                'rosbridge_websocket_launch.xml',
            ])
        )
    )

    return LaunchDescription([
        Node(package='robot_vision', executable='camera_raw_publisher'),
        Node(package='robot_vision', executable='image_compressor'),
        Node(package='robot_vision', executable='test_runner'),
        rosbridge,
        Node(package='web_video_server', executable='web_video_server'),
    ])
