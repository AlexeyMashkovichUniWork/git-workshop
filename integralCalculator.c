#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))

#define NO_STDOUT -1
#define NO_STDIN -2
#define LEFT_BOUND_ERROR 1
#define RIGHT_BOUND_ERROR 2

#define RESULT_ALLOC_ERROR -1

typedef struct interval_s {
	double left_border;
	double right_border;
} interval_t;

typedef struct result_s {
	unsigned int partition_size;
	double rectangle_result;
	double simpson_result;
} integration_result_t;

void free_results(integration_result_t *results);

double rectangle_rule(double left_border, double right_border) {
	return (right_border - left_border) * sin((left_border + right_border) / 2);
}

double simpsons_rule(double left_border, double right_border) {
	return (right_border - left_border) / 6 * (sin(left_border) + 4 * sin((left_border + right_border) / 2) + sin(right_border));
}

double integrate(interval_t interval, unsigned int partition_size, double (*method)(double, double)) {
	double integral_value = 0.0;
	double part_length = (interval.right_border - interval.left_border) / partition_size;
	double part_left_border = interval.left_border;
	for (unsigned int i = 0; i < partition_size; ++i) {
		double part_right_border = part_left_border + part_length;
		integral_value += method(part_left_border, part_right_border);
		part_left_border = part_right_border;
	}
	return integral_value;
}

integration_result_t *allocate_results(unsigned int count) {
	return calloc(count, sizeof(integration_result_t));
}

int calculate_integrals(integration_result_t *results, interval_t interval, unsigned int count, const unsigned int *partition_sizes) {
	if (!results) {
		return RESULT_ALLOC_ERROR;
	}

	for (unsigned int i = 0; i < count; ++i) {
		double integral_rectangle = integrate(interval, partition_sizes[i], rectangle_rule);
		double integral_simpson = integrate(interval, partition_sizes[i], simpsons_rule);

		results[i].partition_size = partition_sizes[i];
		results[i].rectangle_result = integral_rectangle;
		results[i].simpson_result = integral_simpson;
	}

	return 0;
}

void free_results(integration_result_t *results) {
	free(results);
}

int read_interval(interval_t *interval) {
	if (printf("Enter interval's left border: ") < 0) {
		fprintf(stderr, "Cannot write to stdout\n");
		return NO_STDOUT;
	}
	if (scanf("%lf", &(interval->left_border)) != 1) {
		fprintf(stderr, "Cannot read interval's left border\n");
		return NO_STDIN;
	}
	if (interval->left_border < 0) {
		fprintf(stderr, "Left border of the interval must be greater than or equal to 0\n");
		return LEFT_BOUND_ERROR;
	}
	if (printf("Enter interval's right border: ") < 0) {
		fprintf(stderr, "Cannot write to stdout\n");
		return NO_STDOUT;
	}
	if (scanf("%lf", &(interval->right_border)) != 1) {
		fprintf(stderr, "Cannot read interval's right border\n");
		return NO_STDIN;
	}
	if (interval->right_border > M_PI) {
		fprintf(stderr, "Right border of the interval must be less than or equal to pi\n");
		return RIGHT_BOUND_ERROR;
	}
	if (interval->left_border > interval->right_border) {
		fprintf(stderr, "Right border of the interval must be greater than left\n");
		return RIGHT_BOUND_ERROR;
	}
	return 0;
}

int main(int argc, char **argv) {
	interval_t interval = {
		.left_border = 0.0,
		.right_border = 0.0
	};
	if (read_interval(&interval)) {
		return 1;
	}
	unsigned int partition_sizes[] = {5, 10, 20, 100, 500, 1000};
	size_t experiments_count = ARRAY_LENGTH(partition_sizes);
	integration_result_t * results = allocate_results(experiments_count);
	if (!results) {
		return 1;
	}
	if (calculate_integrals(results, interval, experiments_count, partition_sizes) != 0) {
		return 2;
	}

	for (unsigned int i = 0; i < experiments_count; ++i) {

		if (printf("%d %.5f %.5f\n", 
			results[i].partition_size,
			results[i].rectangle_result,
			results[i].simpson_result
		) < 0) {
			fprintf(stderr, "Cannot write result %d to stdout\n", i);
			break;
		}
	}
	free_results(results);
	return 0;
}
