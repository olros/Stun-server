run:
	docker-compose build
	docker-compose up

deploy:
	docker-compose down
	make run
