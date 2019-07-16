import os

import lib


if __name__ == '__main__':
    client = lib.InferenceClient('192.168.2.20:50051')

    # demo for inference
    in_file_name = '../test_data/sample01.jpg'
    result = client.infer(in_file_name)
    print(result)
