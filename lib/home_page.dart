import 'package:flutter/material.dart';
import 'package:webview_flutter/webview_flutter.dart';
import 'dart:async';
import 'dart:io';
import 'dart:convert';

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  bool _isFlashOn = false;
  bool _isVideoEnabled = true;
  late WebViewController _webViewController;
  final HttpClient _httpClient = HttpClient();
  DateTime _lastCommandTime = DateTime.now();

  @override
  void initState() {
    super.initState();
    _initializeWebView();
    _configureHttpClient();
    print('HomePage initialized');
  }

  void _configureHttpClient() {
    _httpClient.connectionTimeout = const Duration(seconds: 5);
    _httpClient.idleTimeout = const Duration(seconds: 5);
    _httpClient.autoUncompress = false;
  }

  void _initializeWebView() {
    print('Initializing WebView');
    _webViewController = WebViewController()
      ..setJavaScriptMode(JavaScriptMode.unrestricted)
      ..setBackgroundColor(const Color(0x00000000))
      ..setNavigationDelegate(
        NavigationDelegate(
          onPageStarted: (String url) {
            print('WebView page started loading: $url');
          },
          onPageFinished: (String url) {
            print('WebView page finished loading: $url');
            _webViewController.runJavaScript('''
              document.body.style.overflow = 'hidden';
              document.querySelector('video').style.objectFit = 'contain';
            ''');
          },
          onWebResourceError: (WebResourceError error) {
            print('WebView error: ${error.description}');
          },
        ),
      )
      ..loadRequest(Uri.parse('http://192.168.4.1/stream'));
    print('WebView initialized');
  }

  @override
  void dispose() {
    _httpClient.close();
    print('HomePage disposed');
    super.dispose();
  }

  Future<void> _sendCommand(String command) async {
    final now = DateTime.now();
    if (now.difference(_lastCommandTime).inMilliseconds < 100) {
      print('Delaying command due to rate limiting');
      await Future.delayed(Duration(milliseconds: 100));
    }
    _lastCommandTime = DateTime.now();

    final url = Uri.parse('http://192.168.4.1/$command');
    print('Preparing to send command: $url');
    try {
      print('Sending command: $url');
      final stopwatch = Stopwatch()..start();

      final request = await _httpClient.getUrl(url);
      request.headers.set('User-Agent', 'Flutter/1.0');
      request.headers.set('Accept', '*/*');
      final response = await request.close();

      final responseBody = await response.transform(utf8.decoder).join();

      stopwatch.stop();
      print('Command round-trip time: ${stopwatch.elapsedMilliseconds}ms');

      if (response.statusCode == 200) {
        print('Command sent successfully: $command');
        print('Response body: $responseBody');
      } else {
        print('Error: Server returned ${response.statusCode}');
        print('Response body: $responseBody');
      }
    } on TimeoutException catch (e) {
      print('Timeout error sending command: $e');
    } catch (e) {
      print('Error sending command: $e');
    }
  }

  void _toggleFlash() {
    setState(() {
      _isFlashOn = !_isFlashOn;
    });
    print('Toggling flash: ${_isFlashOn ? 'ON' : 'OFF'}');
    _sendCommand(_isFlashOn ? 'flash/on' : 'flash/off');
  }

  void _toggleVideo() {
    setState(() {
      _isVideoEnabled = !_isVideoEnabled;
    });
    print('Toggling video: ${_isVideoEnabled ? 'ON' : 'OFF'}');
    if (_isVideoEnabled) {
      _initializeWebView();
    }
  }

  @override
  Widget build(BuildContext context) {
    print('Building HomePage widget');
    return Scaffold(
      appBar: AppBar(
        title: const Text('Controlled Car'),
        actions: [
          IconButton(
            icon: Icon(_isVideoEnabled ? Icons.videocam : Icons.videocam_off),
            onPressed: _toggleVideo,
          ),
        ],
      ),
      body: Column(
        children: [
          if (_isVideoEnabled)
            Expanded(
              flex: 2,
              child: WebViewWidget(controller: _webViewController),
            ),
          Expanded(
            flex: 3,
            child: Center(
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  DirectionButton(
                    icon: Icons.arrow_upward,
                    direction: 'avant',
                    onPressed: _sendCommand,
                    onReleased: () => _sendCommand('stop'),
                  ),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      DirectionButton(
                        icon: Icons.arrow_back,
                        direction: 'gauche',
                        onPressed: _sendCommand,
                        onReleased: () => _sendCommand('stop'),
                      ),
                      const SizedBox(width: 100),
                      DirectionButton(
                        icon: Icons.arrow_forward,
                        direction: 'droite',
                        onPressed: _sendCommand,
                        onReleased: () => _sendCommand('stop'),
                      ),
                    ],
                  ),
                  DirectionButton(
                    icon: Icons.arrow_downward,
                    direction: 'arriere',
                    onPressed: _sendCommand,
                    onReleased: () => _sendCommand('stop'),
                  ),
                  const SizedBox(height: 20),
                  ElevatedButton.icon(
                    onPressed: _toggleFlash,
                    icon: Icon(_isFlashOn ? Icons.flash_on : Icons.flash_off),
                    label: Text(_isFlashOn ? 'Flash On' : 'Flash Off'),
                    style: ElevatedButton.styleFrom(
                      padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 12),
                    ),
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class DirectionButton extends StatefulWidget {
  final IconData icon;
  final String direction;
  final Function(String) onPressed;
  final VoidCallback onReleased;

  const DirectionButton({
    Key? key,
    required this.icon,
    required this.direction,
    required this.onPressed,
    required this.onReleased,
  }) : super(key: key);

  @override
  State<DirectionButton> createState() => _DirectionButtonState();
}

class _DirectionButtonState extends State<DirectionButton> {
  bool _isPressed = false;

  void _handleTapDown(TapDownDetails details) {
    print('Button pressed: ${widget.direction}');
    setState(() {
      _isPressed = true;
    });
    widget.onPressed(widget.direction);
  }

  void _handleTapUp(TapUpDetails details) {
    print('Button released: ${widget.direction}');
    setState(() {
      _isPressed = false;
    });
    widget.onReleased();
  }

  void _handleTapCancel() {
    print('Button tap cancelled: ${widget.direction}');
    setState(() {
      _isPressed = false;
    });
    widget.onReleased();
  }

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTapDown: _handleTapDown,
      onTapUp: _handleTapUp,
      onTapCancel: _handleTapCancel,
      child: Container(
        padding: const EdgeInsets.all(20),
        decoration: BoxDecoration(
          shape: BoxShape.circle,
          color: _isPressed ? Colors.blue.shade700 : Colors.blue,
        ),
        child: Icon(widget.icon, size: 40, color: Colors.white),
      ),
    );
  }
}

