#!/usr/bin/env python3

import cgi
import cgitb
import os

# Enable debugging
cgitb.enable()

def render_form(result=""):
    content = f"""<!DOCTYPE html>
<html lang=\"en\">
<head>
	<meta charset=\"UTF-8\">
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">
	<title>CGI Calculator</title>
	<style>
		body {{
			font-family: Arial, sans-serif;
			background-color: #f4f4f9;
			margin: 0;
			padding: 0;
			display: flex;
			justify-content: center;
			align-items: center;
			height: 100vh;
		}}
		.container {{
			background: #ffffff;
			padding: 10px 20px;
			border-radius: 10px;
			box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
			text-align: center;
			width: 300px;
		}}
		h1 {{
			font-size: 24px;
			margin-bottom: 20px;
			color: #333;
		}}
		form {{
			display: flex;
			flex-direction: column;
			gap: 10px;
		}}
		label {{
			font-size: 14px;
			text-align: left;
			color: #555;
		}}
		input, select, button {{
			padding: 10px;
			border: 1px solid #ccc;
			border-radius: 5px;
			font-size: 16px;
			outline: none;
            text-align: center;
		}}
		button {{
			background-color: #007bff;
			color: #fff;
			cursor: pointer;
			transition: background-color 0.3s ease;
		}}
		button:hover {{
			background-color: #0056b3;
		}}
		.result {{
			margin-top: 15px;
			font-size: 16px;
			color: #007bff;
			font-weight: bold;
		}}
	</style>
</head>
<body>
	<div class=\"container\">
		<h1>Calculator</h1>
		<form method=\"post\" action=\"\">
			<input type=\"text\" id=\"num1\" name=\"num1\" required>

			<select id=\"operator\" name=\"operator\">
				<option value=\"+\">+</option>
				<option value=\"-\">-</option>
				<option value=\"*\">*</option>
				<option value=\"/\">/</option>
			</select>

			<input type=\"text\" id=\"num2\" name=\"num2\" required>

			<button type=\"submit\">Submit</button>
		</form>
		<div class=\"result\">Result: {result if result else "NULL"}</div>
	</div>
</body>
</html>"""
    return content


def calculate():
    form = cgi.FieldStorage()
    try:
        num1 = float(form.getvalue("num1", ""))
        num2 = float(form.getvalue("num2", ""))
        operator = form.getvalue("operator", "")
        
        if operator == "+":
            result = num1 + num2
        elif operator == "-":
            result = num1 - num2
        elif operator == "*":
            result = num1 * num2
        elif operator == "/":
            result = "Error: Division by zero" if num2 == 0 else num1 / num2
        else:
            result = "Error: Invalid operator"
    except (ValueError, TypeError):
        result = "Error: Invalid input"

    return str(result)


def main():
    if "REQUEST_METHOD" in os.environ and os.environ["REQUEST_METHOD"] == "POST":
        result = calculate()
        content = render_form(result)
    else:
        result = ""
        content = render_form()

    content_length = 1804 + len(result)

    print("200 OK")
    print("Content-Type: text/html")
    print(f"Content-Length: {content_length}")
    print()
    print(content)

if __name__ == "__main__":
    main()
