#pragma once
template<typename T>
class UploadBuffer
{
public:
	UploadBuffer(UINT elementCount, bool isConstantBuffer) :
		_isConstantBuffer(isConstantBuffer)
	{
		_elementByteSize = sizeof(T);

		if (isConstantBuffer)
			_elementByteSize = CalcConstantBufferByteSize(sizeof(T));

		ThrowIfFailed(GRAPHIC->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(_elementByteSize * elementCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&_uploadBuffer)));

		ThrowIfFailed(_uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedData)));
	}

	UploadBuffer(const UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
	~UploadBuffer()
	{
		if (_uploadBuffer != nullptr)
			_uploadBuffer->Unmap(0, nullptr);

		_mappedData = nullptr;
	}

	ID3D12Resource* GetResource()const
	{
		return _uploadBuffer.Get();
	}

	void CopyData(int elementIndex, const T& data)
	{
		memcpy(&_mappedData[elementIndex * _elementByteSize], &data, sizeof(T));
	}

	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + 255) & ~255;
	}

private:
	ComPtr<ID3D12Resource> _uploadBuffer;
	BYTE* _mappedData = nullptr;

	UINT _elementByteSize = 0;
	bool _isConstantBuffer;
};

