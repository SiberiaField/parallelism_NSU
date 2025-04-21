import threading
from threading import Thread, Lock, current_thread
from queue import Queue, Empty, ShutDown
import time
import logging
from argparse import ArgumentParser

import cv2
from cv2.typing import MatLike
import numpy as np


MIN_SHAPE = 256

logger = logging.getLogger(__name__)
logger.setLevel(logging.WARNING)
handler = logging.FileHandler('log-file.txt')
formatter = logging.Formatter("%(name)s %(asctime)s %(levelname)s %(message)s")
handler.setFormatter(formatter)
logger.addHandler(handler)
logger_lock = Lock()
stop_sensors = False


def custom_hook(args):
    with logger_lock:
        logger.error(f"{args.thread.name}:{args.exc_type}\n{args.exc_traceback}")


threading.excepthook = custom_hook


class Sensor:
    def get(self):
        raise NotImplementedError("Subclass must implement method get()")


class SensorX(Sensor):
    '''SensorX'''
    def __init__(self, delay):
        self._delay = delay
        self._data = 0

    def get(self) -> int:
        time.sleep(self._delay)
        self._data += 1
        return self._data


class ReadingFrameError(Exception):
    """cv2.VideoCapture.read() returned [False, ...]"""
    def __init__(self, *args):
        super().__init__(*args)


class SensorCam(Sensor):
    def __init__(self, camera_name: str, window_shape: np.typing.NDArray):
        self.cap = cv2.VideoCapture(camera_name)
        if not self.cap.isOpened():
            raise ValueError('Cannot open camera')
        self.window_shape = window_shape
        if (self.window_shape < MIN_SHAPE).any():
            global logger, logger_lock
            with logger_lock:
                logger.warning(f'{current_thread().name}: All sizes of output window must be >= {MIN_SHAPE}')
            self.window_shape = np.array([MIN_SHAPE, MIN_SHAPE], dtype=int)

    def get(self) -> MatLike:
        success, frame = self.cap.read()
        if not success:
            self.cap.release()
            raise ReadingFrameError('Cannot read frame')
        return cv2.resize(frame, self.window_shape)

    def __del__(self):
        self.cap.release()


class SensorThread(Thread):
    def __init__(self, type: str, *sensor_args, **thread_configuration):
        Thread.__init__(self, **thread_configuration)
        if type == 'Cam':
            self.sensor = SensorCam(*sensor_args)
        elif type == 'X':
            self.sensor = SensorX(*sensor_args)
        else:
            raise ValueError("Wrong type of Sensor")
        self.q = Queue()

    def run(self):
        global stop_sensors, logger, logger_lock
        while not stop_sensors:
            try:
                self.q.put(self.sensor.get())
            except ReadingFrameError:
                with logger_lock:
                    logger.exception(f"{current_thread().name}: SensorCam couldn't read next frame. Maybe connection was broken")
                break
        self.q.shutdown(immediate=stop_sensors)
        self.q.join()

    def get(self) -> MatLike | int:
        return self.q.get(False)

    def task_done(self):
        self.q.task_done()


def make_output_image(frame: MatLike, sensor_values: np.ndarray):
    text_org = (frame.shape[0] // 3,
                frame.shape[1] // 5)
    return cv2.putText(frame, str(sensor_values[0]), text_org, cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2)


class WindowImage:
    def __init__(self, freq: int):
        self.freq = freq
        if self.freq <= 0:
            self.freq = 1
            logger.warning(f"{current_thread().name}: The output frequency must be > 0 but got {freq}. Now it is set to {self.freq}")
        self.winname = 'out'
        cv2.namedWindow(self.winname, cv2.WINDOW_AUTOSIZE)

    def show(self, image):
        time.sleep(self.freq)
        cv2.imshow(self.winname, image)

    def __del__(self):
        cv2.destroyAllWindows()


def parse_args():
    parser = ArgumentParser(description='Read frames with different frequency from usb-camera by threads')
    parser.add_argument('--camera_name', type=str,
                        help='System name of a camera or its number')
    parser.add_argument('--window_width', type=int,
                        help="Width of the output window, must be >= 128",
                        default=128)
    parser.add_argument('--window_height', type=int,
                        help="Height of the output window, must be >= 64",
                        default=64)
    parser.add_argument('--freq', type=float,
                        help='Output window frequency',
                        default=0.1)
    return parser.parse_args()


def main():
    global logger, stop_sensors

    args = parse_args()

    camera_shape = np.array([args.window_width, args.window_height], dtype=int)
    camera_name = args.camera_name if args.camera_name is not None else 0
    try:
        sensor_cam = SensorThread('Cam', camera_name, camera_shape)
    except ValueError:
        logger.exception(f'{current_thread().name}: ValueError')
        exit(1)

    sensor_threads = [
        SensorThread('X', 0.01),
        SensorThread('X', 0.1),
        SensorThread('X', 1)
    ]

    sensor_cam.start()
    for sensor_thread in sensor_threads:
        sensor_thread.start()

    window = WindowImage(args.freq)

    sensor_values = np.ndarray((1, 3))
    frame = np.zeros((MIN_SHAPE, MIN_SHAPE, 3), dtype=np.uint8)
    while True:
        try:
            frame = sensor_cam.get()
            sensor_cam.task_done()
        except ShutDown:
            break
        except Empty:
            pass

        for i, sensor_thread in enumerate(sensor_threads):
            try:
                sensor_value = sensor_thread.get()
                sensor_thread.task_done()
                np.put(sensor_values, [i], [sensor_value])
            except ShutDown:
                break
            except Empty:
                pass

        window.show(make_output_image(frame, sensor_values))
        if cv2.waitKey(1) == ord('q'):
            break

    stop_sensors = True

    window.__del__()

    sensor_cam.join()
    for sensor_thread in sensor_threads:
        sensor_thread.join()


if __name__ == "__main__":
    main()
