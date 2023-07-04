import subprocess
import sys
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Initialize the figure and an empty list to store the received numbers
fig, ax = plt.subplots()
line, = ax.plot([], [])
received_numbers = []
ax.set_xlabel('Time')
ax.set_ylabel('Value')

def update_plot(new_number):
    # Append the new number to the list
    received_numbers.append(new_number)

 #   if len(received_numbers) > 100:
        # Limit the list to the latest 10,000 numbers
      #  del received_numbers[0]

    # Update the plot with the new data
    line.set_data(range(len(received_numbers)), received_numbers)
    ax.relim()
    ax.autoscale_view(True, True, True)

    # Refresh the plot
    plt.draw()
    plt.pause(0.000001)

def execute_cmd_command(command):
    try:
        process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, universal_newlines=True)

        while True:
            # Read the latest number from the command output
            new_number = process.stdout.readline().strip()
         #   print(new_number)
            # Call the update_plot() function with each new number received
            update_plot(float(new_number))


    except Exception as e:
        print("An error occurred:", e)

# MAIN


execute_cmd_command("ncat -l 192.168.175.121 3333")

sys.exit()
