import numpy as np
import pandas as pd
import os

data_dir = os.path.join(
    os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'data')


def load_dataset(train_file_path: str, test_file_path: str) -> pd.DataFrame:
    train = pd.read_csv(train_file_path)
    train['Label'] = "Train"
    test = pd.read_csv(test_file_path)
    test['Label'] = "Test"
    trainTest = pd.concat([train, test])
    trainTest = trainTest.drop(["Ticket", "Embarked", "Cabin"], axis=1)
    trainTest = trainTest.reset_index(drop=True)
    return trainTest


def get_missing_values(df: pd.DataFrame) -> pd.DataFrame:
    df1 = pd.DataFrame(index=df.columns)
    number = len(df.index)
    df1['Total'] = df.isna().sum()
    df1['Percent'] = df1.Total / number * 100
    df1 = df1.sort_values(by=["Total"], ascending=False)
    return df1


def substitute_missing_values(df: pd.DataFrame) -> pd.DataFrame:
    df1 = df.copy()
    mean = df1['Age'].mean()
    df1.Age = df.Age.fillna(mean)
    df1.Fare = df.Fare.fillna(15)
    return df1


def get_correlation(df: pd.DataFrame) -> float:
    return df.Age.corr(df.Fare)


def get_survived_per_class(df: pd.DataFrame, group_by_column_name: str) -> pd.Series:
    listA = []
    for _ in df[group_by_column_name].unique():
        countDead = df[(df[group_by_column_name] == _) & (df['Survived'] == 0)][group_by_column_name].count()
        countAlive = df[(df[group_by_column_name] == _) & (df['Survived'] == 1)][group_by_column_name].count()
        percentage = round(countAlive / (countDead + countAlive), 2)
        listA.append(percentage)

    surv = pd.Series(listA, index=df[group_by_column_name].unique()).sort_values(ascending=False)
    return surv


def get_outliers(df: pd.DataFrame) -> (int, pd.DataFrame):
    Q1 = df.Fare.quantile(0.25)
    Q3 = df.Fare.quantile(0.75)
    IQR = Q3 - Q1
    count00 = df[(Q1 - 1.5 * IQR >= df.Fare) | (df.Fare >= Q3 + 1.5 * IQR)].Fare.count()
    people00 = df[df.Fare.notna()]
    outliers = (count00, people00)
    return outliers


def create_new_features(df: pd.DataFrame) -> pd.DataFrame:
    df1 = df.copy()
    df1['Fare_scaled'] = (df1.Fare - df1.Fare.mean()) / df1.Fare.std()

    df1['Age_log'] = np.log(df1.Age)
    df1.Sex = df1.Sex.replace('male', 0)
    df1.Sex = df1.Sex.replace('female', 1)
    return df1


def determine_survival(df: pd.DataFrame, n_interval: int, age: float,
                       sex: str) -> float:
    df1 = df.copy()
    df1 = df[df1.Sex == sex]
    df1 = substitute_missing_values(df1)
    df1['ageInterval'], bins = pd.cut(df1.Age, bins=n_interval, retbins=True)
    df1['a'] = age
    df1['Ibool'] = pd.cut(df1.a, bins=bins) == df1['ageInterval']
    df1 = df1[df1.Ibool == True]
    surv = df1[df1.Survived == 1].PassengerId.count()
    nots = df1[df1.Survived == 0].PassengerId.count()

    if surv + nots == 0:
        return np.nan
    else:
        sp = surv / (surv + nots)
        return sp
