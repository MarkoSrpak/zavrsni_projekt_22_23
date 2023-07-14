import subprocess
import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Initialize
fig, ax = plt.subplots()
line, = ax.plot([], [])
received_numbers = []
ax.set_xlabel('Time [s]')
ax.set_ylabel('Value')

def update_plot(new_number):
    # Append the new number to the list
    received_numbers.append(new_number)

    if len(received_numbers) > 512:
        # Limit the list
        del received_numbers[0]

    # Update plot
    line.set_data(np.arange(0, len(received_numbers) * (1/512), (1/512)), received_numbers)
    ax.relim()
    ax.autoscale_view(True, True, True)

    # Refresh plot
    plt.draw()
    plt.pause(0.001)

def execute_cmd_command(command):
    try:
        process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, universal_newlines=True)
        while True:
            # Read number
            new_number = process.stdout.readline().strip()
            update_plot(float(new_number))


    except Exception as e:
        print("An error occurred:", e)


# MAIN
execute_cmd_command("ncat -l 192.168.175.121 3333")
sys.exit()
