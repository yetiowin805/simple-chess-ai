import os
import sys
import select
import time
import cProfile
import pstats

os.environ["TF_CPP_MIN_LOG_LEVEL"] = "3"
import keras.api._v2.keras as keras
from keras.api._v2.keras import Input, regularizers
from keras.api._v2.keras.models import Model
from keras.api._v2.keras.layers import Activation, Dense, BatchNormalization
from keras.api._v2.keras.optimizers import Adam
from trainer import Trainer

NUM_GAMES = 30
ITERATIONS = 25

if __name__ == "__main__":

    with cProfile.Profile() as pr:

        if len(sys.argv) < 2:
            input_layer = Input(shape=(70,))
            layer_1 = Activation("relu")(
                BatchNormalization()(
                    Dense(
                        units=100,
                        kernel_regularizer=regularizers.L2(1e-4),
                    )(input_layer)
                )
            )
            layer_2 = Activation("relu")(
                BatchNormalization()(
                    Dense(
                        units=100,
                        kernel_regularizer=regularizers.L2(1e-4),
                    )(layer_1)
                )
            )
            layer_3 = Activation("relu")(
                BatchNormalization()(
                    Dense(
                        units=100,
                        kernel_regularizer=regularizers.L2(1e-4),
                    )(layer_2)
                )
            )
            layer_4 = Activation("relu")(
                BatchNormalization()(
                    Dense(
                        units=100,
                        kernel_regularizer=regularizers.L2(1e-4),
                    )(layer_3)
                )
            )
            eval_layer = Activation("relu")(
                BatchNormalization()(
                    Dense(
                        units=100,
                        kernel_regularizer=regularizers.L2(1e-4),
                    )(layer_4)
                )
            )
            eval_output = Dense(units=1, activation="tanh")(eval_layer)
            prob_layer = Activation("relu")(
                BatchNormalization()(
                    Dense(
                        units=100,
                        kernel_regularizer=regularizers.L2(1e-4),
                    )(layer_4)
                )
            )
            prob_output = Dense(units=96, activation="softmax")(prob_layer)

            model = Model(inputs=input_layer, outputs=[eval_output, prob_output])
            model.compile(
                optimizer=Adam(learning_rate=0.01),
                loss=[
                    keras.losses.MeanSquaredError(),
                    keras.losses.CategoricalCrossentropy(),
                ],
            )

            model.save(f"./training/models/model_0")

            model_num = 0
        else:
            model_num = int(sys.argv[1])
            model = keras.models.load_model(f"./training/models/model_{model_num}")

        fail_num = 0

        try:
            os.mkdir("./training/logs")
            print("Created directory ./training/logs")
        except FileExistsError:
            print("Will write logs into directory ./training/logs")

        trained_model = model

        while True:

            old_weights = model.get_weights()

            trainer = Trainer(
                model, trained_model, iterations=ITERATIONS, num_games=NUM_GAMES
            )

            print(f"Begin training generation {model_num}! (Times failed: {fail_num})")

            start_time = time.time()

            res = trainer.train_generation()

            print(
                f"Training generation {model_num} took {(time.time()-start_time)/60:.1f} minutes!"
            )

            start_time = time.time()
            # Write game logs
            open(
                f"./training/logs/model_{model_num}_{fail_num}_training.txt",
                "w",
                encoding="utf-8",
            ).write(res[1])

            print(f"Writing training logs took {time.time()-start_time:.1f} seconds!")

            start_time = time.time()

            old_model = keras.models.load_model(f"./training/models/model_{model_num}")

            print(f"Loading model took {time.time()-start_time:.1f} seconds!")

            TEST_NUM = 10

            tester = Trainer(
                trainer.model2,
                old_model,
                num_games=TEST_NUM,
                iterations=ITERATIONS,
                test=True,
            )

            start_time = time.time()

            res = tester.train_generation()

            print(
                f"New model {model_num+1} scored {res[0]*100:.1f}% out of {TEST_NUM} games!\nTesting took {time.time()-start_time:.0f} seconds!"
            )

            # Write game logs
            open(
                f"./training/logs/model_{model_num}_{fail_num}_testing.txt",
                "w",
                encoding="utf-8",
            ).write(res[1])

            # New neural net scores >50%
            if res[0] > 0.5:
                trainer.model2.save(f"./training/models/model_{model_num+1}")
                model = trainer.model2
                trained_model = trainer.model2
                model_num += 1
                fail_num = 0
            else:
                trainer.model2.save(
                    f"./training/models/model_{model_num}_failed_{fail_num}"
                )
                model = old_model
                trained_model = trainer.model2
                fail_num += 1

            stats = pstats.Stats(pr)
            stats.sort_stats(pstats.SortKey.TIME)
            stats.dump_stats(filename="train.prof")
