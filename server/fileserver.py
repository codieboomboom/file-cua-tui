#!/usr/bin/env python3
from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import os
from urllib.parse import urlparse, parse_qs
from datetime import datetime

class FileServerHandler(BaseHTTPRequestHandler):
    """Implement Handlers for the server"""
    
    def do_GET(self):
        """
        Define GET HTTP method handlers
        """
        parsed = urlparse(self.path)
        path = parsed.path
        params = parse_qs(parsed.query)
        
        # Get the requested path or use current directory
        target_path = params.get('path', ['.'])[0]
        
        # routing to endpoint handler
        if path == '/list':
            self.handle_list(target_path)
        elif path == '/read':
            self.handle_read(target_path)
        elif path == '/info':
            self.handle_info(target_path)
        else:
            self.send_error(404, "Unknown endpoint")
    
    def handle_list(self, target_path):
        """To handle /list endpoint"""
        try:
            items = []
            # List out all files and directories directly under this path
            for item in os.listdir(target_path):
                full_path = os.path.join(target_path, item)
                stat = os.stat(full_path)
                items.append({
                    'name': item,
                    'type': 'directory' if os.path.isdir(full_path) else 'file',
                    'size': stat.st_size if os.path.isfile(full_path) else None,
                    'modified': datetime.fromtimestamp(stat.st_mtime).isoformat()
                })
            
            response = {
                'success': True,
                'path': os.path.abspath(target_path),
                'items': items
            }
            
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps(response).encode())
            
        except Exception as e:
            self.send_error(500, str(e))
    
    def handle_read(self, target_path):
        """To Handle /read endpoint, return the content of the file"""
        try:
            with open(target_path, 'r') as f:
                content = f.read()
            
            response = {
                'success': True,
                'path': target_path,
                'content': content
            }
            
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps(response).encode())
            
        except Exception as e:
            self.send_error(500, str(e))
    
    def handle_info(self, target_path):
        """To Handle /info endpoint, return the metadata of the file being requested"""
        try:
            stat = os.stat(target_path)
            
            response = {
                'success': True,
                'path': os.path.abspath(target_path),
                'name': os.path.basename(target_path),
                'type': 'directory' if os.path.isdir(target_path) else 'file',
                'size': stat.st_size,
                'modified': datetime.fromtimestamp(stat.st_mtime).isoformat(),
                'created': datetime.fromtimestamp(stat.st_ctime).isoformat()
            }
            
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps(response).encode())
            
        except Exception as e:
            self.send_error(500, str(e))

if __name__ == '__main__':
    server = HTTPServer(('localhost', 8000), FileServerHandler)
    print("Server running on http://localhost:8000")
    print("Endpoints:")
    print("  /list?path=.")
    print("  /read?path=file.txt")
    print("  /info?path=file.txt")
    server.serve_forever()