from spyre import server
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

class VHIlab(server.App):
    title = "NOAA logo National Oceanic and Atmospheric Administration"
    inputs = [
        {
            'type':'dropdown',
            'label':'Unit',
            'options':[
                {'label':'VHI','value':'VHI'},
                {'label':'TCI','value':'TCI'},
                {'label':'VCI','value':'VCI'},
            ],
                "variable_name": 'index',
                "action_id": "update",
        }
        ,{
            "type": 'dropdown',
            "label": 'Province',
            "options": [
                {"label": "Вінницька", "value": 1},
                {"label": "Волинська", "value": 2},
                {"label": "Дніпропетровська", "value":3},
                {"label": "Донецька", "value": 4},
                {"label": "Житомирська", "value": 5},
                {"label": "Закарпатська", "value": 6},
                {"label": "Запорізька", "value": 7},
                {"label": "Івано-Франківська", "value": 8},
                {"label": "Київська", "value": 9},
                {"label": "Кіровоградська", "value": 10},
                {"label": "Луганська", "value": 11},
                {"label": "Львівська", "value": 12},
                {"label": "Миколаївська", "value": 13},
                {"label": "Одеська", "value": 14},
                {"label": "Полтавська", "value": 15},
                {"label": "Рівенська", "value": 16},
                {"label": "Сумська", "value": 17},
                {"label": "Тернопільська", "value": 18},
                {"label": "Харківська", "value": 19},
                {"label": "Херсонська", "value": 20},
                {"label": "Хмельницька", "value": 21},
                {"label": "Черкаська", "value": 22},
                {"label": "Чернівецька", "value": 23},
                {"label": "Чернігівська", "value": 24},
                {"label": "Республіка Крим", "value": 25},
            ],
            "variable_name": 'area',
            "action_id": "update",
        },
        {
            'type':'slider',
            'label':'Year_min',
            'min':1982,
            'max':2024,
            'value':1982,
            'variable_name':'year',
            'action_id':'update'
        },
        {
            'type':'slider',
            'label':'Year_max',
            'min':1982,
            'max':2024,
            'value':2024,
            'variable_name':'year1',
            'action_id':'update'
        },
        {
            'type':'text',
            'label':'Week (1-52)',
            'variable_name':'week',
            'value':'1-52',
            'action_id':'update'
        }
    ]
    tabs = ['Table','Plot']

    controls = [{
            "type": "hidden",
            "label": "get data",
            'id':'update'
        }]

    outputs = [
        {
            'id':'table_output',
            'type':'table',
            'control_id':'update',
            'tab':'Table'
        },
        {
            'id':'plot_output',
            'type':'plot',
            'control_id':'update',
            'tab':'Plot'
        }
    ]

    def table_output(self, params):
        df = pd.read_csv('./VHI_index.csv', index_col=0)
        week_range = params['week'].split('-')
        week_range_1 = int(week_range[0])
        week_range_2 = int(week_range[1])
        index = params['index']
        table = df[['Year','Week',index,'area']][(df.Year.between(int(params['year']), int(params['year1'])))
                                                 & (df.Week.between(week_range_1, week_range_2))
                                                & (df['area']==int(params['area']))]
        return table

    def plot_output(self, params):
        y = params['index']
        table = self.table_output(params)
        plt.figure(figsize=(16,8))
        plt.title(f'{y} depence on time')
        plt.ylabel(y)
        plt.xlabel('week')
        sns.set_style('darkgrid')
        graph = sns.lineplot(data=table, x='Week',y=y)
        return graph
        
app = VHIlab()
app.launch() 