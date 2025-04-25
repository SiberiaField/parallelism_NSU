from threading import Thread, Lock
from queue import SimpleQueue, PriorityQueue, Empty
from timeit import default_timer as timer
from argparse import ArgumentParser
from dataclasses import dataclass, field
from typing import Any
import logging

from ultralytics import YOLO
from cv2.typing import MatLike
import cv2


NUM_OF_THREADS = 12
model = YOLO('yolov8s-pose')
res_queue = PriorityQueue()
predict_lock = Lock()
end_of_video = False

logger = logging.getLogger(__name__)
logger.setLevel(logging.WARNING)
handler = logging.FileHandler('log-file.txt')
formatter = logging.Formatter("%(name)s %(asctime)s %(levelname)s %(message)s")
handler.setFormatter(formatter)
logger.addHandler(handler)


@dataclass(order=True)
class PrioritizedItem:
    priority: int
    item: Any = field(compare=False)


class InferThread(Thread):
    def __init__(self, **thread_configuration):
        Thread.__init__(self, **thread_configuration)
        self.frames_queue = SimpleQueue()

    def run(self):
        global res_queue, end_of_video
        while True:
            try:
                frame_id, frame = self.frames_queue.get(False)
            except Empty:
                if end_of_video:
                    break
                else:
                    continue
            res = self.predict(frame)
            res_queue.put(PrioritizedItem(priority=frame_id, item=res), False)

    def predict(self, frame: MatLike) -> MatLike:
        global predict_lock
        with predict_lock:
            pred = model.predict(frame, verbose=False)
        return pred[0].plot()

    def put_frame(self, frame_id: int, frame: MatLike):
        self.frames_queue.put((frame_id, frame), False)


def init_threads(num_of_threads: int) -> list[InferThread]:
    threads = list()
    while num_of_threads > 0:
        threads.append(InferThread())
        num_of_threads -= 1
    return threads


def start_threads(threads: list[InferThread]):
    for thread in threads:
        thread.start()


def join_threads(threads: list[InferThread]):
    for thread in threads:
        thread.join()


class OutputVideo:
    def __init__(self, output_video_name: str):
        fourcc = cv2.VideoWriter_fourcc(*'mp4v')
        self.__video_writer__ = cv2.VideoWriter(output_video_name + '.mp4', fourcc, 30, (640,  480), isColor=True)

    def __del__(self):
        self.__video_writer__.release()

    def write(self, frame: MatLike):
        self.__video_writer__.write(frame)


def save_video_from_threads(output_video_name: str):
    output_video = OutputVideo(output_video_name)
    frame_counter = 0
    while True:
        try:
            res = res_queue.get(False)
        except Empty:
            break
        frame_counter += 1
        output_video.write(res.item)
    print(f'saved {frame_counter} frames')


def proceed_with_single_thread(input_video_path: str, output_video_name: str):
    print("predicting and saving...")
    start = timer()
    output_video = OutputVideo(output_video_name)
    results_gen = model.predict(input_video_path, True, verbose=False)
    for result in results_gen:
        output_video.write(result.plot())
    end = timer()
    print(f"\nSpeed: {end - start}s")


def parse_args():
    parser = ArgumentParser(description="Use threads to speed up yolov8s-pose inference on CPU")
    parser.add_argument('input_video_path', type=str, help='Path to input video')
    parser.add_argument('--multithreading', type=bool, help='Use multithreading for speed up or not use', default=False)
    parser.add_argument('--output_video_name', type=str, help='Name of output video', default='out')
    return parser.parse_args()


def main():
    args = parse_args()

    if args.multithreading is False:
        proceed_with_single_thread(args.input_video_path, args.output_video_name)
        exit(0)

    cap = cv2.VideoCapture(args.input_video_path)
    if not cap.isOpened():
        logger.error('Cannot open video')
        exit(1)

    threads = init_threads(NUM_OF_THREADS)
    start_threads(threads)

    thread_id = 0
    frame_id = 0
    start = timer()
    while True:
        success, frame = cap.read()
        if success:
            threads[thread_id].put_frame(frame_id, frame)
            thread_id += 1
            frame_id += 1
            thread_id %= NUM_OF_THREADS
        else:
            break
    print(f'read {frame_id} frames')
    global end_of_video
    end_of_video = True
    cap.release()

    print('waiting for threads...')
    join_threads(threads)

    print('saving video...')
    save_video_from_threads(args.output_video_name)
    end = timer()

    print(f"\nSpeed: {end - start}s")


if __name__ == '__main__':
    main()
