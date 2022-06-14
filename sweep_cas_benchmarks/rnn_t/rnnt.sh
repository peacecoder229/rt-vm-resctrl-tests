#/bin/bash
total_cores=$1
instances=$((total_cores/4))

start=0
for ((i=1; i <=$instances;i++)); do
  numactl -C $start-$((start+4-1)) -m 0 /root/anaconda3/envs/pytorch/bin/python3 -u /root/workspace/benchmark/pytorch_model/models/language_modeling/pytorch/rnnt/inference/cpu/inference.py --batch_size 1 --ckpt /home/dataset/pytorch/rnnt/results/rnnt.pt --dataset_dir /home/dataset/pytorch/rnnt/dataset/LibriSpeech --ipex --jit --model_toml /root/workspace/benchmark/pytorch_model/models/language_modeling/pytorch/rnnt/inference/cpu/configs/rnnt.toml --val_manifest /home/dataset/pytorch/rnnt/dataset/LibriSpeech/librispeech-dev-clean-wav.json --warm_up 10 2>&1 | tee /home/dataset/pytorch/rnn_t_log_inference_latency_avx_fp32_bs_1_real_20220513141835_1_20220513141836_instance_0_cores_$start-$((start+4-1)).log &
  start=$((start+4))
done

# waiting for the jobs to finish before we copy results back
for job in `jobs -p`
do
 echo "Waiting for $job to finish ...."
 wait $job
done



