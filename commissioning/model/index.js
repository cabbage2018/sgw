'use strict'
let fs = require("fs");
let modelJsonObject = require('../../public/modeling/bigrefrigerator.xlsm.extracted.json')
module.exports = {
    modelJsonObject
}
/*
{
	"version": "2021/5/20 14:54:07",
	"license": "2029-11-23T23:11:58.875Z",
	"machines": [
		{
			"modelId": "SENTRON4200",
			"description": "2021/5/20 14:54:07",
			"category": "PROJECT_PLACEHOLDER_2021",
			"dataPoints": [
				{
					"Id": 1000,
					"description_CN": "功率因数",
					"description": "Cos phi",
					"unit": "1",
					"datatype": "Float",
					"scaling": 1,
					"uri": "功率因数",
					"protocolData": {
						"functioncodes": 3,
						"register": 69,
						"quantity": 2,
						"offset": 0,
						"bits": 0,
						"display": "{1: 'X'}{0: 'failure'}"
					}
                },
                ...

                */