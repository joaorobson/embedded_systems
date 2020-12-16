from datetime import datetime
import csv
from constants import commands_names, CENTRAL_TOPIC

class CSV:

    def __init__(self):
		pass

    def data_to_command(self, topic, data):
		if topic == CENTRAL_TOPIC:
			return "Leitura dos sensores da GPIO"
		else:
			commands = []
			for key, value in data.items():
				command = commands_names.get(key, "")
				commands.append(command)
			return " ".join(commands)
		
	
	def write_on_csv(self, topic, data):
       with open(FILENAME, 'a') as f:
            writer = csv.writer(f)

            # Check if csv file has a header
            if os.path.getsize(FILENAME) == 0:
                fields = ["Data/Hora", "Tópico", "Dado", "Comando"]
                writer.writerow(fields)

            date_hour = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
			command = self.data_to_command(topic, data)
            writer.writerow([date_hour, topic, data, command])
