import socket
import wave
import datetime

SERVER_IP = "192.168.4.1"
TCP_PORT = 80

# WAVE_OUTPUT_FILENAME = "output.wav"
FRAME_RATE = 8000
MAX_FRAMES = 800


def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((SERVER_IP, TCP_PORT))

    print(f"Listening for data on TCP port {TCP_PORT}")
    frames = []
    received_frames = 0
    while True:
        data = sock.recv(1024)
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
