import socket
import wave
import datetime

LOCAL_IP = "x.x.x.x"
UDP_PORT = 9999

# WAVE_OUTPUT_FILENAME = "output.wav"
FRAME_RATE = 44100
MAX_FRAMES = 800


def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((LOCAL_IP, UDP_PORT))

    print(f"Listening for data on UDP port {UDP_PORT}")
    frames = []
    received_frames = 0
    while True:
        data, addr = sock.recvfrom(2048)
        # print(f"Received data from {addr}")
        print(f"Frame {received_frames}")
        received_frames += 1
        frames.append(data)
        if received_frames > MAX_FRAMES:
            break

    print("Reached max frames. Writing file...")
    output_filename = (
        f"./recordings/output_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}.wav"
    )
    wf = wave.open(output_filename, "wb")
    wf.setnchannels(1)
    wf.setsampwidth(1)
    wf.setframerate(FRAME_RATE)
    wf.writeframes(b"".join(frames))
    wf.close()
    print(f"Saved to {output_filename}")


if __name__ == "__main__":
    main()
