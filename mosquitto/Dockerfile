# Start from the official Eclipse Mosquitto image
FROM eclipse-mosquitto:2.0.18

# Copy the configuration file to the appropriate directory
COPY mosquitto.conf /mosquitto/config/mosquitto.conf

# Copy the password file to the appropriate directory
COPY passwd /mosquitto/config/passwd

# Expose the MQTT port (1883)
EXPOSE 1883

# Run Mosquitto using the configuration file
CMD ["/usr/sbin/mosquitto", "-c", "/mosquitto/config/mosquitto.conf"]