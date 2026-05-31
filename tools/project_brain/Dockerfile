FROM python:3.11-alpine

# Set execution directory
WORKDIR /app

# Copy the AIPBF python source engine into the container
COPY analyzer.py reviewer.py generator.py project_brain.py /app/

# Set entrypoint to project_brain.py CLI
ENTRYPOINT ["python", "/app/project_brain.py"]
