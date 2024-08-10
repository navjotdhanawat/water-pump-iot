const fs = require("fs");
const path = require("path");

// Logger utility
class Logger {
  constructor(logFilePath = "app.log") {
    this.logFilePath = path.resolve(logFilePath);
  }

  // Method to get the current timestamp
  getTimestamp() {
    return new Date().toISOString();
  }

  // Method to log a message with a timestamp
  log(message) {
    const timestamp = this.getTimestamp();
    const logMessage = `[${timestamp}] ${message}`;

    // Log to the console
    console.log(logMessage);

    // Append the log message to a file
    fs.appendFile(this.logFilePath, logMessage + "\n", (err) => {
      if (err) throw err;
    });
  }
}

// Exporting an instance of Logger
module.exports = new Logger();
