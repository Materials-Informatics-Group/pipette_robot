from flask import Flask, render_template, request, jsonify
import serial

# Flask app instance
app = Flask(__name__)

# Serial connection settings for Arduino
DEFAULT_PORT = "/dev/ttyACM0"
BAUD_RATE = 9600

# Global serial handle (initialized on startup)
ser = None

def init_serial():
    """Initialize the serial connection to the device."""
    global ser
    try:
        ser = serial.Serial(DEFAULT_PORT, BAUD_RATE, timeout=1)
        print(f"Serial initialized on {DEFAULT_PORT} ({BAUD_RATE} baud)", flush=True)
    except Exception as e:
        print(f"Failed to open {DEFAULT_PORT}: {e}", flush=True)
        ser = None


@app.route("/")
def index():
    """Serve the main GUI page."""
    return render_template("index.html")

def json_to_command(data):
    """Convert GUI JSON payload into a single-line command for the device."""
    assert isinstance(data, dict)
    data_type = data.get("type")

    # XY move buttons: send command on press, send RELEASED on release
    if data_type == "xy":
        payload = data.get("payload")
        assert isinstance(payload, dict)
        button_state = payload.get("state")
        assert isinstance(button_state, str)
        if button_state == "pressed":
            cmd = payload.get("command")
            assert isinstance(cmd, str)
            return cmd
        elif button_state == "released":
            return "RELEASED"

    # Lift buttons: same behavior as XY
    elif data_type == "lift":
        payload = data.get("payload")
        assert isinstance(payload, dict)
        button_state = payload.get("state")
        assert isinstance(button_state, str)
        if button_state == "pressed":
            cmd = payload.get("command")
            assert isinstance(cmd, str)
            return cmd
        elif button_state == "released":
            return "RELEASED"

    # Syringe commands: convert float value into discrete "ticks"
    elif data_type == "syringes":
        payload = data.get("payload")
        assert isinstance(payload, dict)
        cmd = payload.get("command")
        value = payload.get("value")
        assert isinstance(value, (int, float))
        ticks = round(value * 5)  # 0.2 step -> integer ticks
        if cmd == "PULL":
            return cmd + " " + str(ticks)
        elif cmd == "PUSH":
            return cmd + " " + str(ticks)
        elif cmd == "PUSHALL":
            return "PUSH -1"  # special value meaning "push all"

    # Emergency stop
    elif data_type == "halt":
        return "HALT"

@app.route("/send", methods=["POST"])
def send_command():
    """Receive a GUI command, forward it to the device via serial, and return the reply."""
    global ser

    # Parse request body (prefer JSON, fallback to form)
    data = request.get_json()
    if data is None:
        data = request.form

    # Translate GUI payload into device command string
    cmd = json_to_command(data)
    if cmd is None:
        cmd = ""
    cmd.strip()  # NOTE: this does not modify cmd in-place (strings are immutable)

    # Reject empty commands
    if cmd == "":
        return jsonify({"status": "error", "message": "No command provided."}), 400

    # Ensure serial is available
    if ser is None or not ser.is_open:
        return jsonify({
            "status":"error",
            "message": "Serial port is not open."
        }), 500

    try:
        # Clear stale input before sending a new command
        ser.reset_input_buffer()

        # Send command line to Arduino
        ser.write((cmd + "\n").encode("utf-8"))
        ser.flush()

        # Read one-line reply from Arduino
        reply = ser.readline().decode("utf-8", errors="ignore").strip()

        # Return round-trip info to the GUI
        return jsonify({
                "status": "ok",
                "sent": cmd,
                "received": reply
            })

    except Exception as e:
        # Return error as JSON for the GUI to display/log
        return jsonify({"status": "error", "message": str(e)}), 500

if __name__ == "__main__":
    # Initialize serial connection once, then start the web server
    init_serial()
    app.run(debug=True, use_reloader=False)
