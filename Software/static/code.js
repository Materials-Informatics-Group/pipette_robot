// Wait until the DOM is fully loaded before initializing the interface
document.addEventListener("DOMContentLoaded", () => {
    console.log("Robot interface open");

    // Get control buttons and input elements from the GUI
    const xyButtons = document.querySelectorAll("button[data-xy]");
    const liftButtons = document.querySelectorAll("button[data-lift]");
    const syringesButtons = document.querySelectorAll("button[data-syringes]");
    const syringesInputs = document.querySelectorAll("input[data-spinbox]");

    // Get syringe-related UI elements
    const pullSpinBox = document.getElementById("pullValue");
    const pushSpinBox = document.getElementById("pushValue");
    const aspiratedText = document.getElementById("aspiratedValue");
    const remainingText = document.getElementById("remainingValue");

    // Log area and emergency halt button
    const messageLog = document.getElementById("messageLog");
    const haltButton = document.getElementById("haltButton");

    // Initialize syringe state values from UI
    let aspiratedAmount = Number(document.getElementById("aspiratedValue").textContent);
    let remainingAmount = Number(document.getElementById("remainingValue").textContent);

    // Append a message to the log area
    function appendToLog(text) {
        if (text.length == 0) return;
        console.log(`appendToLog: ${text}`);
        messageLog.value += text + "\n";
        messageLog.scrollTop = messageLog.scrollHeight;
    }

    // Send a command to the server
    async function sendCommand(command) {
        if (!command) {
            console.log("No command to send");
            return;
        }

        console.log("Send command");

        try {
            const res = await fetch("/send", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify(command)
            });

            const data = await res.json();

            // Check response status
            if (data.status === "ok") {
                console.log("status is ok")
            }

            // Log received message from the server
            console.log(`received : ${data.received}`);
            appendToLog(data.received)

        } catch (err) {
            console.log(`Fetch error: ${err}`);
        }
    }

    // XY movement button handling
    xyButtons.forEach(btn => {
        const type = "xy";
        const cmd = btn.getAttribute("data-xy");

        // Command template for XY control
        const data = {
            type: type,
            payload: {
                command: cmd,
                state: null,
            },
        }

        // Send "pressed" state when button is held down
        btn.addEventListener("pointerdown", () => {
            data.payload.state = "pressed"
            console.log(`XY: ${cmd} pressed`);
            sendCommand(data)
        });

        // Send "released" state when button is released
        btn.addEventListener("pointerup", () => {
            data.payload.state = "released"
            console.log(`XY: ${cmd} released`);
            sendCommand(data)
        })
    })

    // Lift control button handling
    liftButtons.forEach(btn => {
        const type = "lift";
        const cmd = btn.getAttribute("data-lift");

        // Command template for lift control
        const data = {
            type: type,
            payload: {
                command: cmd,
                state: null,
            },
        };

        // Send "pressed" state
        btn.addEventListener("pointerdown", () => {
            data.payload.state = "pressed";
            console.log(`Lift: ${cmd} pressed`);
            sendCommand(data);
        });

        // Send "released" state
        btn.addEventListener("pointerup", () => {
            data.payload.state = "released";
            console.log(`Lift: ${cmd} released`);
            sendCommand(data);
        })
    })

    // Update spinbox limits after pulling liquid
    function updateSpinboxPull(value) {
        pullSpinBox.value = 0.0;
        pullSpinBox.max = (Number(pullSpinBox.max) - value).toFixed(1);
        pushSpinBox.max = (Number(pushSpinBox.max) + value).toFixed(1);
    }

    // Update spinbox limits after pushing liquid
    function updateSpinboxPush(value) {
        pullSpinBox.max = (Number(pullSpinBox.max) + value).toFixed(1);
        pushSpinBox.max = (Number(pushSpinBox.max) - value).toFixed(1);
        pushSpinBox.value = Math.min(value, Number(pushSpinBox.max));
    }

    // Update syringe values after pull
    function updateSyringesValuesPull(value) {
        aspiratedAmount += value;
        remainingAmount -= value;
        aspiratedAmount = Math.abs(aspiratedAmount);
    }

    // Update syringe values after push
    function updateSyringesValuesPush(value) {
        aspiratedAmount -= value;
        remainingAmount += value;
        remainingAmount = Math.abs(remainingAmount);
    }

    // Set syringe status directly (e.g., initialization)
    function setSyringesStatus(aspirated) {
        aspiratedAmount = aspirated;
        remainingAmount = 5.0 - aspirated;
        pullSpinBox.max = (remainingAmount).toFixed(1);
        pushSpinBox.max = (aspiratedAmount).toFixed(1);
        updateSyringesStatus();
    }

    // Reflect syringe values in the UI
    function updateSyringesStatus() {
        aspiratedText.textContent = aspiratedAmount.toFixed(1);
        remainingText.textContent = remainingAmount.toFixed(1);
    }

    // Clamp input value within min/max and round to 0.2 step
    function clampInputValue(box) {
        let value = box.value;
        value = Math.min(value, Number(box.max));
        value = Math.max(value, Number(box.min));
        value = Math.round(value * 5) / 5;
        box.value = value;
    }

    // Syringe button handling (PULL / PUSH / PUSHALL)
    syringesButtons.forEach(btn => {
        btn.addEventListener("click", () => {
            const type = "syringes";
            const cmd = btn.getAttribute("data-syringes");

            // Select corresponding spinbox
            const spinBox = (cmd === "PULL") ? pullSpinBox : pushSpinBox;
            let value = Number(spinBox.value);

            // For PUSHALL, use maximum possible value
            if (cmd === "PUSHALL") {
                value = Number(pushSpinBox.max);
            }

            // Build command object
            const data = {
                type: type,
                payload: {
                    command: cmd,
                    value: value,
                },
            };

            console.log(`Syringes: ${cmd} ${value}`);
            sendCommand(data);

            // Update internal state and UI after command
            if (cmd === "PULL") {
                updateSpinboxPull(value);
                updateSyringesValuesPull(value);
            }
            else {
                updateSpinboxPush(value);
                updateSyringesValuesPush(value);
            }
            updateSyringesStatus();
        })
    })

    // Handle manual input in spinboxes
    syringesInputs.forEach(box => {
        let previousValue = 0.0;
        let enterPushed = false;

        // Store previous value on focus
        box.addEventListener("focus", () => {
            previousValue = box.value;
            enterPushed = false;
        });

        // Confirm value on Enter key
        box.addEventListener("keydown", (e) => {
            if (e.key === "Enter") {
                e.preventDefault();

                enterPushed = true;
                clampInputValue(box);
                box.blur();
            };
        });

        // Restore previous value if Enter was not pressed
        box.addEventListener("blur", () => {
            if (!enterPushed) {
                box.value = previousValue;
            }
        });
    });

    // Emergency halt button handling
    haltButton.addEventListener("click", () => {
        const type = "halt";
        const data = {
            type: type,
        };
        console.log("Halt button clicked");
        sendCommand(data);
    })
});
