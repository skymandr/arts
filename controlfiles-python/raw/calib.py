import pyarts
import numpy as np

# Set some data
colddata = [8832.6767578125, 1002.3690185546875, 998.2736206054688, 1004.0936889648438, 1005.7428588867188, 1000.1062622070312, 1006.3336791992188, 1005.427978515625, 1008.0487060546875, 1008.4397583007812, 1007.8699951171875, 1006.3451538085938, 1001.8883056640625, 1007.7392578125, 1012.2906494140625, 1006.6441040039062, 1010.3069458007812, 1014.4865112304688, 1013.8457641601562, 1005.9954223632812, 1010.4893188476562, 1013.8729858398438, 1011.614013671875, 1007.9857788085938, 1018.01123046875, 1004.7649536132812, 1011.1383056640625, 1025.95166015625, 1008.281982421875, 1012.9866943359375, 1014.08251953125, 1020.9761352539062, 1017.7366333007812, 1016.4536743164062, 1013.0831298828125, 1014.3846435546875, 1017.9910278320312, 1021.705078125, 1018.4212036132812, 1014.6141357421875, 1011.8338012695312, 1020.6993408203125, 1021.0669555664062, 1024.3104248046875, 1019.2828369140625, 1019.8271484375, 1013.8621826171875, 1016.4443359375, 1022.431640625, 1021.034423828125, 1019.310302734375, 1028.4385986328125, 1013.056884765625, 1019.739013671875, 1032.4132080078125, 1024.4923095703125, 1026.392822265625, 1024.583984375, 1025.745361328125, 1030.054443359375, 1021.537353515625, 1013.72314453125, 1026.8779296875, 1023.4873657226562, 1033.5048828125, 1016.8510131835938, 1024.19287109375, 1027.5654296875, 1030.9010009765625, 1021.8408813476562, 1023.4039916992188, 1027.0172119140625, 1030.074462890625, 1030.5001220703125, 1021.06591796875, 1033.2958984375, 1035.279052734375, 1025.0853271484375, 1025.51904296875, 1026.8282470703125, 1030.858642578125, 1030.55517578125, 1028.03564453125, 1032.01708984375, 1032.7969970703125, 1022.7337646484375, 1032.3712158203125, 1030.2386474609375, 1036.47314453125, 1024.4033203125, 1034.822998046875, 1036.37451171875, 1031.82177734375, 1038.3343505859375, 1030.930908203125, 1024.4998779296875, 1032.6334228515625, 1039.4324951171875, 1035.0257568359375, 1032.9244384765625, 1031.08349609375, 1032.54736328125, 1036.113037109375, 1039.7677001953125, 1031.7010498046875, 1031.888427734375, 1033.5848388671875, 1029.05712890625, 1037.3602294921875, 1039.6590576171875, 1039.541015625, 1032.3885498046875, 1034.822998046875, 1038.482421875, 1034.920166015625, 1043.2467041015625, 1034.43701171875, 1040.5859375, 1041.6905517578125, 1041.08251953125, 1031.1856689453125, 1033.457763671875, 1033.59033203125, 1038.62841796875, 1037.605712890625, 1040.8477783203125, 1044.083251953125, 1043.848388671875, 1106.6624755859375, 1044.1158447265625, 1037.8170166015625, 1042.9033203125, 1035.346435546875, 1039.2332763671875, 1038.147216796875, 1047.002685546875, 1041.4443359375, 1038.5552978515625, 1030.85205078125, 1040.0599365234375, 1036.0244140625, 1040.498291015625, 1036.6864013671875, 1039.5599365234375, 1039.7056884765625, 1049.0667724609375, 1045.1768798828125, 1044.3558349609375, 1047.7984619140625, 1038.296630859375, 1046.894775390625, 1048.0986328125, 1042.474609375, 1048.777587890625, 1047.8870849609375, 1049.0494384765625, 1050.5079345703125, 1050.403076171875, 1044.8203125, 1047.3052978515625, 1044.8570556640625, 1048.1307373046875, 1051.205078125, 1052.98388671875, 1049.7972412109375, 1046.0133056640625, 1047.0111083984375, 1045.352294921875, 1048.20166015625, 1050.678466796875, 1046.88134765625, 1046.3533935546875, 1049.1656494140625, 1046.239501953125, 1056.678466796875, 1049.4969482421875, 1056.279052734375, 1047.0037841796875, 1045.4512939453125, 1055.8004150390625, 1050.401611328125, 1050.224609375, 1047.837158203125, 1048.4862060546875, 1050.740234375, 1055.5244140625, 1056.3502197265625, 1047.3128662109375, 1057.3826904296875, 1053.65478515625, 1057.37451171875, 1058.8087158203125, 1052.721435546875, 1057.22802734375, 1044.54150390625, 1051.290771484375, 1055.4925537109375, 1053.0592041015625, 1058.4786376953125, 1055.1893310546875, 1051.9122314453125, 1044.9776611328125, 1051.3204345703125, 1058.7694091796875, 1055.3046875, 1052.462158203125, 1054.6036376953125, 1049.842529296875, 1055.151611328125, 1054.512451171875, 1053.656005859375, 1063.6754150390625, 1057.255615234375, 1060.9686279296875, 1060.4056396484375, 1052.6412353515625, 1052.854736328125, 1053.9437255859375, 1056.9075927734375, 1057.686767578125, 1062.494384765625, 1062.393310546875, 1059.0814208984375, 1056.1063232421875, 1062.3863525390625, 1062.243408203125, 1059.981689453125, 1059.1419677734375, 1061.866943359375, 1058.201416015625, 1057.3504638671875, 1051.3426513671875, 1061.7996826171875, 1062.3858642578125, 1058.222412109375, 1059.2335205078125, 1056.941162109375, 1055.4111328125, 1054.33447265625, 1062.13525390625, 1062.8531494140625, 1063.16748046875, 1067.5147705078125, 1056.0147705078125, 1056.868896484375, 1060.30029296875, 1059.189453125, 1066.971435546875, 1063.62939453125, 1064.0675048828125, 1060.9093017578125, 1057.1358642578125, 1060.138671875, 1064.766845703125, 1063.234619140625, 1060.4647216796875, 1059.6884765625, 1062.2291259765625, 1065.5909423828125, 1064.943115234375, 1059.6820068359375, 1061.9781494140625, 1064.0423583984375, 1066.0858154296875, 1064.655517578125, 1069.2967529296875, 1059.17138671875, 1065.6478271484375, 1061.2662353515625, 1059.7142333984375, 1060.958984375, 1065.5577392578125, 1056.595947265625, 1056.689208984375, 1057.638427734375, 1064.06689453125, 1058.8870849609375, 1064.5155029296875, 1059.3590087890625, 1063.899658203125, 1065.66650390625, 1065.8768310546875, 1062.5811767578125, 1070.4844970703125, 1067.979248046875, 1065.44140625, 1067.3157958984375, 1070.8782958984375, 1064.5999755859375, 1062.412353515625, 1067.445556640625, 1062.7862548828125, 1071.5396728515625, 1066.9305419921875, 1070.4840087890625, 1074.3404541015625, 1062.927978515625, 1066.7161865234375, 1070.2374267578125, 1066.860595703125, 1064.06787109375, 1063.71533203125, 1064.46630859375, 1066.18115234375, 1061.2080078125, 1076.132080078125, 1061.6380615234375, 1070.4307861328125, 1066.5435791015625, 1057.7198486328125, 1063.8568115234375, 1067.9088134765625, 1068.2462158203125, 1073.1461181640625, 1066.3363037109375, 1064.842041015625, 1064.6710205078125, 1063.8944091796875, 1067.4915771484375, 1058.856689453125, 1071.743408203125, 1071.644287109375, 1071.9453125, 1070.8731689453125, 1066.50732421875, 1070.4593505859375, 1059.82861328125, 1073.2489013671875, 1072.372802734375, 1069.7178955078125, 1068.92724609375, 1071.1466064453125, 1069.1231689453125, 1073.399169921875, 1075.579833984375, 1064.8187255859375, 1069.44091796875, 1075.371826171875, 1071.5789794921875, 1060.8541259765625, 1065.13525390625, 1068.068359375, 1068.4697265625, 1071.721923828125, 1069.3468017578125, 1065.223388671875, 1074.5167236328125, 1072.6395263671875, 1065.98974609375, 1067.8956298828125, 1069.2650146484375, 1075.9022216796875, 1076.9345703125, 1069.1513671875, 1067.135986328125, 1072.1046142578125, 1068.5523681640625, 1076.6422119140625, 1073.463623046875, 1071.518310546875, 1071.0667724609375, 1069.9276123046875, 1063.7528076171875, 1073.093017578125, 1074.148193359375, 1067.749267578125, 1071.2041015625, 1070.066162109375, 1072.7354736328125, 1065.5150146484375, 1065.02294921875, 1072.625732421875, 1067.411865234375, 1066.65478515625, 1070.2437744140625, 1068.33447265625, 1074.9100341796875, 1074.4990234375, 1064.280517578125, 1074.3492431640625, 1067.24609375, 1076.8699951171875, 1064.6195068359375, 1069.3687744140625, 1072.1278076171875, 1071.9881591796875, 1071.1326904296875, 1069.044189453125, 1074.0062255859375, 1071.1973876953125, 1072.819580078125, 1076.542236328125, 1073.5147705078125, 1070.81640625, 1072.3834228515625, 1073.210205078125, 1066.3504638671875, 1067.23193359375, 1065.951171875, 1066.4443359375, 1064.6658935546875, 1062.837158203125, 1078.874755859375, 1070.2113037109375, 1074.703857421875, 1071.927490234375, 1061.7032470703125, 1070.246337890625, 1067.40576171875, 1063.399169921875]
hotdata = [9425.927734375, 1634.466552734375, 1620.264892578125, 1619.6658935546875, 1619.498291015625, 1633.0810546875, 1628.6534423828125, 1627.307373046875, 1629.3831787109375, 1629.50146484375, 1630.8392333984375, 1646.474365234375, 1640.033203125, 1637.4638671875, 1626.3653564453125, 1632.8392333984375, 1640.183349609375, 1639.9239501953125, 1644.6263427734375, 1641.1363525390625, 1636.8653564453125, 1638.4693603515625, 1645.0185546875, 1644.0599365234375, 1634.80078125, 1645.5140380859375, 1649.7034912109375, 1649.3447265625, 1643.55322265625, 1653.277587890625, 1650.3212890625, 1646.507568359375, 1644.2059326171875, 1655.443359375, 1650.7857666015625, 1654.6109619140625, 1648.9366455078125, 1661.956298828125, 1651.741943359375, 1643.900634765625, 1649.857666015625, 1655.2987060546875, 1659.102783203125, 1645.071044921875, 1653.05419921875, 1669.087158203125, 1654.4296875, 1647.93212890625, 1656.7183837890625, 1651.767333984375, 1649.0216064453125, 1657.5565185546875, 1657.9300537109375, 1661.6639404296875, 1660.3414306640625, 1657.545166015625, 1651.772216796875, 1651.4227294921875, 1670.078125, 1657.3023681640625, 1661.5198974609375, 1654.5267333984375, 1657.597412109375, 1647.980224609375, 1659.9508056640625, 1660.7022705078125, 1658.96826171875, 1645.84619140625, 1661.4747314453125, 1659.7742919921875, 1671.9866943359375, 1655.1707763671875, 1658.2889404296875, 1660.008056640625, 1660.708740234375, 1654.5601806640625, 1662.591064453125, 1655.906494140625, 1673.5782470703125, 1672.8521728515625, 1673.6334228515625, 1662.054443359375, 1657.127197265625, 1665.910888671875, 1669.5777587890625, 1667.779541015625, 1664.721923828125, 1667.3455810546875, 1674.74658203125, 1671.7655029296875, 1666.2747802734375, 1681.2886962890625, 1678.5584716796875, 1665.858642578125, 1659.443115234375, 1670.467529296875, 1679.2593994140625, 1671.940185546875, 1668.83447265625, 1679.6600341796875, 1691.9561767578125, 1680.31640625, 1671.447509765625, 1681.278076171875, 1668.8267822265625, 1680.427734375, 1668.239990234375, 1681.8468017578125, 1669.03369140625, 1679.4869384765625, 1670.8970947265625, 1671.2587890625, 1679.902099609375, 1671.9698486328125, 1672.1983642578125, 1671.7349853515625, 1671.6064453125, 1676.8128662109375, 1678.4671630859375, 1679.4793701171875, 1684.042724609375, 1690.36767578125, 1692.494873046875, 1670.04931640625, 1684.307861328125, 1676.817138671875, 1681.798583984375, 1689.2391357421875, 1766.0743408203125, 1686.2947998046875, 1687.509521484375, 1686.39501953125, 1684.2489013671875, 1684.21923828125, 1678.4974365234375, 1680.6522216796875, 1687.4208984375, 1694.35595703125, 1690.50830078125, 1689.6890869140625, 1683.0177001953125, 1695.2301025390625, 1690.0902099609375, 1691.98388671875, 1687.3492431640625, 1691.8297119140625, 1687.6937255859375, 1682.5103759765625, 1690.9166259765625, 1691.7611083984375, 1696.9139404296875, 1690.8375244140625, 1692.7410888671875, 1690.742919921875, 1709.0775146484375, 1696.3790283203125, 1689.9150390625, 1686.166748046875, 1696.48583984375, 1707.3446044921875, 1694.277099609375, 1695.2740478515625, 1700.458740234375, 1692.1656494140625, 1694.060791015625, 1700.0113525390625, 1690.5611572265625, 1701.517822265625, 1694.39453125, 1699.5523681640625, 1705.9881591796875, 1707.9654541015625, 1697.766357421875, 1694.3780517578125, 1691.602294921875, 1698.4144287109375, 1702.3155517578125, 1690.340087890625, 1714.321044921875, 1692.334228515625, 1701.1915283203125, 1706.482177734375, 1699.25927734375, 1702.308837890625, 1708.1976318359375, 1700.637939453125, 1709.171630859375, 1710.2254638671875, 1708.052490234375, 1706.2379150390625, 1706.9453125, 1711.0609130859375, 1706.4886474609375, 1696.056884765625, 1702.6239013671875, 1702.1976318359375, 1696.76611328125, 1704.9951171875, 1697.6783447265625, 1699.794921875, 1704.92529296875, 1700.8507080078125, 1715.3101806640625, 1704.8026123046875, 1711.3570556640625, 1725.562255859375, 1706.150634765625, 1708.4339599609375, 1712.3818359375, 1710.3233642578125, 1711.6878662109375, 1719.116455078125, 1711.181396484375, 1703.4451904296875, 1696.355712890625, 1703.7373046875, 1709.191650390625, 1704.158203125, 1706.618896484375, 1710.13232421875, 1706.109130859375, 1705.19384765625, 1715.7366943359375, 1701.2236328125, 1714.4337158203125, 1711.5672607421875, 1719.3016357421875, 1710.6915283203125, 1723.3895263671875, 1728.060302734375, 1716.514404296875, 1701.261474609375, 1709.6094970703125, 1702.6185302734375, 1699.2276611328125, 1710.8741455078125, 1711.574462890625, 1713.1844482421875, 1717.029296875, 1706.6190185546875, 1720.369140625, 1715.56640625, 1711.55224609375, 1701.1363525390625, 1716.4693603515625, 1711.6290283203125, 1709.3074951171875, 1721.413330078125, 1713.2554931640625, 1714.848876953125, 1716.8687744140625, 1713.265869140625, 1719.5277099609375, 1711.483642578125, 1727.899658203125, 1702.524169921875, 1710.478271484375, 1715.8431396484375, 1722.24267578125, 1711.529541015625, 1716.3770751953125, 1709.9996337890625, 1723.0126953125, 1719.989013671875, 1714.970947265625, 1732.3289794921875, 1721.4656982421875, 1715.072021484375, 1715.6751708984375, 1728.765869140625, 1709.861328125, 1712.2244873046875, 1718.1031494140625, 1718.8380126953125, 1716.842041015625, 1716.08203125, 1715.1956787109375, 1721.6436767578125, 1728.6214599609375, 1714.0616455078125, 1715.383056640625, 1716.5015869140625, 1711.4234619140625, 1724.7357177734375, 1725.8914794921875, 1715.0594482421875, 1715.4931640625, 1716.5582275390625, 1722.8436279296875, 1729.1439208984375, 1723.496826171875, 1732.5091552734375, 1717.4039306640625, 1722.636474609375, 1722.064208984375, 1719.3135986328125, 1734.9847412109375, 1717.016845703125, 1723.2222900390625, 1722.705810546875, 1737.3529052734375, 1726.8887939453125, 1731.1517333984375, 1726.0142822265625, 1720.6800537109375, 1725.673583984375, 1716.5504150390625, 1727.9176025390625, 1719.257568359375, 1735.7698974609375, 1728.831787109375, 1726.59716796875, 1728.782958984375, 1730.11376953125, 1721.6533203125, 1721.4232177734375, 1721.5128173828125, 1730.2957763671875, 1722.4154052734375, 1728.0941162109375, 1734.0260009765625, 1736.7625732421875, 1721.2076416015625, 1731.8304443359375, 1724.247314453125, 1737.25732421875, 1730.746826171875, 1726.646728515625, 1729.059326171875, 1727.6400146484375, 1731.4281005859375, 1739.509765625, 1721.2386474609375, 1734.8416748046875, 1733.207763671875, 1731.494140625, 1732.234375, 1720.40576171875, 1731.009033203125, 1728.149169921875, 1725.5118408203125, 1745.0977783203125, 1738.4732666015625, 1722.6109619140625, 1731.828369140625, 1729.4373779296875, 1737.064697265625, 1734.6297607421875, 1736.0350341796875, 1728.0107421875, 1724.2388916015625, 1728.1668701171875, 1733.546142578125, 1732.08251953125, 1731.7977294921875, 1735.7906494140625, 1733.174560546875, 1727.75830078125, 1732.0418701171875, 1735.4676513671875, 1734.5465087890625, 1746.862548828125, 1738.9371337890625, 1723.5452880859375, 1735.94921875, 1747.727294921875, 1738.6009521484375, 1725.5927734375, 1733.2894287109375, 1738.37939453125, 1736.6666259765625, 1735.9945068359375, 1722.9300537109375, 1729.6671142578125, 1731.3873291015625, 1732.1348876953125, 1725.9371337890625, 1742.05517578125, 1721.6514892578125, 1722.7000732421875, 1738.1435546875, 1735.03076171875, 1731.075439453125, 1723.944580078125, 1726.07861328125, 1740.5078125, 1722.42333984375, 1733.8133544921875, 1730.969970703125, 1728.30908203125, 1732.2747802734375, 1747.771240234375, 1736.557373046875, 1730.284423828125, 1735.9412841796875, 1726.932373046875, 1733.6925048828125, 1722.1468505859375, 1735.7554931640625, 1730.458251953125, 1731.2283935546875, 1720.7716064453125, 1733.9046630859375, 1737.7747802734375, 1717.0052490234375, 1723.216796875, 1729.11376953125, 1729.6552734375, 1728.2080078125, 1728.4676513671875]
atmdata = [8617.5078125, 712.2105712890625, 716.8009033203125, 716.2568969726562, 720.9293212890625, 716.0878295898438, 719.8970336914062, 716.9266357421875, 724.1128540039062, 718.9920043945312, 717.8422241210938, 722.42626953125, 722.9622802734375, 723.2803344726562, 723.1168823242188, 722.0239868164062, 718.8621215820312, 720.9759521484375, 721.3329467773438, 723.5121459960938, 722.0167236328125, 721.5424194335938, 721.8936157226562, 724.89501953125, 720.6084594726562, 723.4158935546875, 725.0005493164062, 724.4153442382812, 719.9954223632812, 723.5985107421875, 721.5783081054688, 718.8128051757812, 722.2979125976562, 729.7347412109375, 726.2683715820312, 724.8579711914062, 728.8424682617188, 721.7559814453125, 725.38232421875, 718.6768188476562, 726.79638671875, 731.1763305664062, 727.4586791992188, 730.3949584960938, 728.80517578125, 728.3001708984375, 730.5973510742188, 728.5784912109375, 726.0672607421875, 727.4721069335938, 727.2244873046875, 729.117919921875, 726.7540283203125, 724.9216918945312, 724.7778930664062, 727.4959716796875, 729.5992431640625, 732.8597412109375, 728.3992919921875, 731.9542846679688, 729.499755859375, 730.1336669921875, 731.524169921875, 728.48681640625, 734.6986083984375, 734.462158203125, 735.9940185546875, 732.0958251953125, 732.5751342773438, 726.7548828125, 731.062744140625, 732.3743896484375, 735.00146484375, 734.732666015625, 732.828369140625, 730.0160522460938, 733.5352783203125, 733.0144653320312, 734.3453369140625, 730.3237915039062, 740.1720581054688, 736.009033203125, 731.2778930664062, 737.6637573242188, 740.575927734375, 730.7047119140625, 740.4791259765625, 731.0487670898438, 736.1978759765625, 743.0575561523438, 736.6866455078125, 740.7904052734375, 736.6676635742188, 738.70947265625, 738.4393920898438, 737.3626708984375, 731.1744384765625, 734.9500122070312, 731.8639526367188, 736.7522583007812, 738.1738891601562, 735.0634155273438, 736.546875, 738.3018798828125, 737.3016357421875, 739.3574829101562, 740.9872436523438, 739.5280151367188, 736.5314331054688, 739.3282470703125, 736.2167358398438, 734.3364868164062, 739.3088989257812, 741.7991333007812, 739.34521484375, 734.591796875, 744.8935546875, 738.6709594726562, 739.6410522460938, 737.3272705078125, 738.8810424804688, 738.2230834960938, 747.283935546875, 741.067138671875, 736.954833984375, 738.9760131835938, 740.5963745117188, 743.0333251953125, 810.9816284179688, 745.7802734375, 744.6368408203125, 746.7295532226562, 742.4557495117188, 741.8038940429688, 743.869384765625, 741.7761840820312, 744.510986328125, 741.1808471679688, 740.417724609375, 739.1649780273438, 743.180419921875, 748.0718994140625, 746.0810546875, 747.0963134765625, 752.493408203125, 745.0317993164062, 743.2501831054688, 740.6632080078125, 745.1278076171875, 743.8634033203125, 741.4481201171875, 742.3663940429688, 744.0455932617188, 740.5894775390625, 745.6445922851562, 745.7504272460938, 743.8857421875, 748.5211791992188, 742.33251953125, 748.3526000976562, 747.2259521484375, 748.9059448242188, 750.666015625, 743.6796264648438, 744.7589111328125, 750.9620971679688, 748.5497436523438, 751.3572998046875, 749.5416870117188, 747.2323608398438, 752.2841796875, 751.7308959960938, 749.210205078125, 749.3551025390625, 751.1453857421875, 751.468505859375, 749.4379272460938, 749.76708984375, 753.79296875, 746.6013793945312, 745.0316772460938, 753.2785034179688, 753.9444580078125, 753.562255859375, 753.9602661132812, 756.8409423828125, 748.1854858398438, 756.6119995117188, 753.0153198242188, 748.2387084960938, 750.8028564453125, 757.6018676757812, 754.7648315429688, 750.5410766601562, 757.200927734375, 750.2025146484375, 749.9137573242188, 755.9161987304688, 754.850341796875, 754.9622802734375, 754.8758544921875, 749.1746826171875, 752.101318359375, 756.8346557617188, 756.4663696289062, 753.631103515625, 756.0628051757812, 751.3597412109375, 753.1941528320312, 752.6746215820312, 752.380126953125, 753.9412231445312, 752.109375, 756.138671875, 754.5698852539062, 748.4758911132812, 757.8302001953125, 756.7368774414062, 751.2572631835938, 757.2349243164062, 757.9310302734375, 751.06640625, 754.0440063476562, 757.3995971679688, 751.301513671875, 754.14501953125, 758.3526611328125, 760.5264892578125, 753.7992553710938, 764.7600708007812, 750.4473266601562, 757.7088623046875, 755.254638671875, 756.09326171875, 758.1901245117188, 752.13720703125, 763.2262573242188, 756.4882202148438, 757.3125, 758.1299438476562, 759.1998291015625, 756.8063354492188, 758.4133911132812, 760.1993408203125, 760.7335205078125, 759.8297119140625, 760.3846435546875, 750.5161743164062, 755.8385620117188, 759.6460571289062, 755.4339599609375, 759.4839477539062, 755.4797973632812, 758.6928100585938, 757.3972778320312, 761.864013671875, 757.3914184570312, 755.6781616210938, 759.7369995117188, 756.1593017578125, 760.362060546875, 757.1483154296875, 763.9395751953125, 759.0450439453125, 758.3504028320312, 760.7550659179688, 761.736083984375, 754.724365234375, 757.7305908203125, 755.944580078125, 766.7095336914062, 760.2844848632812, 763.3430786132812, 754.6065673828125, 767.7559814453125, 757.4431762695312, 759.2725830078125, 763.2352294921875, 756.5181274414062, 758.6492309570312, 753.8316650390625, 759.1116333007812, 759.9368896484375, 754.354248046875, 763.63818359375, 762.4686889648438, 762.7760009765625, 762.2753295898438, 759.1504516601562, 764.0830078125, 755.67431640625, 760.5575561523438, 760.77587890625, 756.0087890625, 758.3549194335938, 762.5340576171875, 759.2042846679688, 758.4063110351562, 760.3629760742188, 761.9218139648438, 762.928955078125, 765.4620971679688, 759.613525390625, 763.8079223632812, 760.2044067382812, 759.2621459960938, 770.0529174804688, 765.272705078125, 762.7382202148438, 762.5174560546875, 762.469482421875, 765.3027954101562, 762.2269897460938, 760.9755859375, 765.9793701171875, 760.7219848632812, 763.6327514648438, 762.2977294921875, 765.5328979492188, 763.4185791015625, 762.1271362304688, 769.2151489257812, 768.1387329101562, 760.3922119140625, 766.3328857421875, 766.00634765625, 764.892333984375, 763.5142822265625, 764.0322875976562, 763.6100463867188, 762.0741577148438, 766.2770385742188, 768.2127685546875, 770.3229370117188, 762.997802734375, 767.8787231445312, 767.31640625, 768.2726440429688, 764.3598022460938, 767.5139770507812, 767.555419921875, 765.4375, 762.1907958984375, 766.4506225585938, 763.3803100585938, 761.1818237304688, 766.2160034179688, 768.8074340820312, 763.1947021484375, 771.815673828125, 760.4871215820312, 760.8411254882812, 763.9328002929688, 766.240966796875, 772.1505126953125, 777.4149780273438, 765.4288940429688, 765.03564453125, 766.6122436523438, 768.9070434570312, 763.6024169921875, 771.65234375, 769.26611328125, 768.703369140625, 765.9913940429688, 763.5110473632812, 770.2511596679688, 765.5018310546875, 767.9469604492188, 770.6702880859375, 771.7215576171875, 768.9676513671875, 769.5408325195312, 765.0084228515625, 762.3637084960938, 766.7791137695312, 765.095947265625, 768.4645385742188, 767.9328002929688, 769.5963134765625, 762.8157348632812, 766.1392211914062, 770.1456909179688, 765.89111328125, 769.5460205078125, 768.6026000976562, 765.3101806640625, 771.7474365234375, 769.3365478515625, 769.9991455078125, 770.9095458984375, 762.8317260742188, 766.9445190429688, 765.8038940429688, 769.4593505859375, 771.0072631835938, 768.8228149414062, 763.3833618164062, 766.17919921875, 769.3993530273438, 767.9537963867188, 771.7418823242188, 764.4359741210938, 766.7808837890625, 768.813232421875, 773.0010986328125, 769.7052612304688, 762.0250244140625, 763.4954833984375, 769.6172485351562]
hottemp = 112.94170379638672
coldtemp = 59.834102630615234

# Set the workspace
ws = pyarts.workspace.Workspace()
ws.cold=pyarts.classes.Vector(colddata)
ws.hot=pyarts.classes.Vector(hotdata)
ws.atm=pyarts.classes.Vector(atmdata)
ws.tcold=pyarts.classes.Numeric(coldtemp)
ws.thot=pyarts.classes.Numeric(hottemp)

ws.yColdAtmHot(ws.y, ws.cold, ws.atm, ws.hot, ws.tcold, ws.thot)

ws.y.value -= [40.5722607081147, 35.455556189581614, 35.80106312956708, 35.001399925753006, 35.18949181408409, 36.00450021619277, 35.39013663623524, 35.19650507948537, 35.565125693102146, 35.08313873952886, 35.109480221403345, 36.27909123526318, 36.621361809309974, 35.844360410169415, 34.82521541482963, 35.69547401821437, 35.261130230237505, 34.91131940953065, 35.20643562857436, 36.214133479584376, 35.37581142158019, 34.978090391665035, 35.54258852305194, 36.19806750001249, 34.226749640198854, 36.51487513260994, 36.03686127370363, 34.14586311411777, 35.73383382670161, 35.83139949819706, 35.41843080484182, 34.180444362726455, 34.788914039333825, 36.00436781315183, 35.948292983237344, 35.81749068919366, 35.4960520140115, 34.953904856053576, 35.26110042767361, 34.85895852678799, 36.108260752726075, 35.60484335425386, 35.39530066234025, 34.68891001478558, 35.493192497173204, 35.988034461722044, 36.3494338280266, 35.62482314618548, 35.02008679915626, 35.11620668465545, 35.20062868691166, 34.566656382028775, 36.2560431843116, 35.443336531545796, 33.815565764565534, 34.91870591483945, 34.63021013700029, 35.118376493668805, 35.32605748867286, 34.59466586378022, 35.599978733849284, 36.331180472238366, 34.9648380627702, 34.746911810357815, 34.50249160315826, 36.541463976327876, 35.72234602642856, 34.45456167844611, 34.708775677464615, 35.268357588393485, 35.89645779900158, 34.92335727034148, 34.889406741960705, 34.88207386121019, 35.90261422597111, 33.90879906185796, 34.288779527474695, 35.24522760055309, 35.97279959959076, 35.459401103806954, 35.8168765878527, 35.06346572244128, 34.78195840908801, 35.17319032296609, 35.46282614820378, 35.79090596203202, 35.319717662655506, 34.8944009859386, 34.84967077832267, 36.75336199746586, 34.75965205604794, 35.49324734845462, 35.59714948756857, 34.476744008025186, 35.119351495569056, 36.22739687902785, 35.075178476137495, 34.268667602669154, 34.43180729412717, 35.51350836353769, 36.29594310642159, 35.44476352585066, 34.79337063074265, 34.87717730279589, 35.2944460607321, 35.879322114929906, 35.34969349085013, 36.279507116283426, 34.542085946434085, 34.90575958483623, 34.319459772264565, 35.05781925544082, 35.505233695051444, 34.96204188145565, 35.202353631941946, 33.75259530601629, 35.7008695760327, 34.632433987737514, 34.64296955973772, 34.565004572972555, 36.05616014454576, 35.96598105709129, 36.757852628961565, 34.80679393307237, 35.14445685524653, 34.62583584048871, 34.5603492227016, 35.080778402945384, 36.02060861929254, 35.16202889120935, 35.86877175780512, 35.39077520701697, 35.86328643181915, 35.344017661043466, 35.42809908131211, 34.252379011044695, 35.42235332063733, 35.75232792860554, 36.45181775241462, 35.23574610829296, 35.796384808889144, 36.114368989773865, 36.21417167043133, 36.027432989471265, 36.28232770708739, 34.71353779539991, 34.87817703740708, 34.56062125172603, 34.84007619627647, 35.90526953324055, 34.87862436451552, 34.57235791242583, 35.46124986352416, 34.338751975469854, 35.55762639069083, 34.95113238621694, 34.3668080271662, 34.61683386735299, 35.18280469871627, 35.779988761923576, 35.494733854730455, 35.27831926860454, 35.25063148910621, 34.134985022548186, 34.68934008899216, 35.874609205271156, 35.20421230399565, 36.039239452383924, 35.288621788195584, 34.998314449710946, 36.096904611241065, 36.18475427703165, 35.27366982716714, 35.50779418750093, 34.27807796617758, 35.44336554846828, 34.61014680409328, 35.29712605464694, 36.67671811960258, 34.03685711404921, 34.914440779982826, 35.80375826091948, 35.8743139637908, 35.878517799870615, 35.86103266546951, 35.245620038860594, 34.7646280369236, 36.54532256852366, 34.991669601078854, 34.97916163360394, 34.7694211864827, 35.30927431003779, 35.63013491583912, 34.33837232597291, 36.64556457374566, 35.26826067420397, 34.52734141922777, 35.62842243768105, 34.60729125407508, 35.099072871137054, 35.677021603551985, 35.88223163743343, 35.90179105744609, 35.01334978843593, 35.64306702325767, 36.25633087935417, 35.500036881811454, 35.76499980531417, 35.43437834165441, 35.391259806956754, 35.51911093815805, 34.73765775502892, 35.05210857522812, 34.63662668450709, 34.294042254787385, 35.02441240052151, 35.9621474451628, 35.55912368100005, 34.850150978042564, 35.377998210774486, 34.70318501530542, 34.11255432587346, 35.16392025654057, 35.24384991448662, 34.49703283841545, 34.635018565828865, 35.53816827247084, 35.49404825068004, 35.10216403019144, 36.56955208691163, 35.10749304591868, 35.84005044955621, 34.7034677282483, 34.4269870004457, 34.97627956151308, 34.80627608037212, 36.00626802281705, 35.69952853166935, 36.03110329393729, 34.78305612725529, 35.30800799051539, 34.89521193133513, 34.34547285524411, 35.48202963212723, 35.99082128082262, 35.33453595314195, 35.42498670384151, 34.15393157085241, 34.671885764129264, 34.99151566137044, 35.1022963310091, 35.74195755223208, 35.29668835065188, 34.69967301512823, 35.39730125059045, 35.135506025407736, 35.16519767739829, 34.92615082506263, 35.09774942111139, 34.47203661949361, 35.62779273979612, 34.85224379257967, 35.64828116598682, 34.89738588499135, 34.819887795226265, 35.12050923292229, 35.98356495363763, 34.40788760439871, 35.201110821159205, 35.72164742840373, 35.75207762228494, 34.76349201096006, 36.29096305703829, 35.60558426832574, 36.48022374963067, 34.859145424317354, 35.5897035404078, 35.48531766280122, 35.80294308305577, 34.90013500037631, 34.34483322158758, 34.7942257628271, 35.062732973297976, 34.17282963592421, 35.26726362560669, 35.06546445617915, 34.882015806560815, 34.45130466809043, 35.1902014928435, 36.07108212471748, 34.59610150128974, 35.8680098127258, 34.28087000320937, 34.65161208573777, 34.39375440524426, 34.15971620718753, 35.83309900591246, 34.65559839296837, 34.631851148594755, 35.14143607759158, 36.08076691785024, 35.94968210841273, 35.54978588652263, 35.49716625420016, 35.594137806128245, 33.926286148603836, 36.18912563091269, 35.18537469148198, 35.11519956232649, 36.71266645445686, 35.764086978914406, 35.43610074059768, 35.229949757061846, 34.599027377375876, 35.4928512475927, 35.23533675005114, 35.49426858833873, 35.79892358324379, 35.34835266276626, 36.389494325111784, 35.00636645609389, 35.68608807977919, 34.98368710947884, 34.88710613097351, 35.59725433879679, 35.58572562187675, 36.487951059129266, 34.65914499058082, 34.897965650962576, 35.12501763120896, 35.23604917360743, 35.609411015429615, 35.32827497114339, 35.499936650355615, 34.59113301541026, 36.179756206430056, 35.62579474744565, 34.54969512264566, 35.09201900272221, 36.48822021112115, 35.902680088714476, 36.095106169901015, 35.557008073416796, 34.926283376424905, 35.30640675003999, 35.52428918213961, 35.12176127589835, 35.45943252135581, 35.83465257141647, 36.01386896798434, 34.79729769716971, 34.181725057392754, 34.518110850236994, 35.56786521510765, 36.26426271653931, 36.25328453350172, 35.612581363185456, 34.418239646807066, 35.08967666977455, 35.629005204024786, 35.22341225249948, 36.433502151388424, 36.67082512965287, 34.98154267938668, 35.10541868069729, 36.07250827194042, 35.88599919894894, 35.15977096587665, 35.32953843320858, 36.56270260964917, 36.642440346629684, 35.52401617280744, 35.70171960548428, 35.67208633132377, 35.103056503437344, 35.708053043534875, 34.560987312924155, 35.48745058040342, 35.892801517557295, 34.87123331186396, 35.73568666714405, 34.760949004653234, 36.36848817037571, 35.21209483457089, 35.261317138726135, 35.73297233300943, 34.8966958203688, 36.08342613595413, 35.20523101832387, 35.491336772384216, 35.52052124038504, 35.013344643223405, 35.27880168654656, 35.27114822328942, 35.589689669860704, 35.28349979711183, 36.15661980502759, 35.19476651829859, 36.06571258221163, 36.07654860924022, 36.193907273906575, 36.33727577511285, 34.34047367203425, 35.694882483713144, 34.54205511949001, 35.4589694582556, 36.59905905352115, 35.010516378590744, 35.40932812867757, 36.37478446113313]

assert np.all(np.isclose(ws.y.value, 0)), "Calibration fails to produce expected values"
