# Description: server to test the flight controller client
from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/heartbeat', methods=['GET'])
def heartbeat():
    return jsonify({'status': 'ok'})

if __name__ == '__main__':
    app.run(host='192.168.69.162')